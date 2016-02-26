#include "BitMRC.h"

BitMRC::BitMRC()
{
	//known nodes stream 1
	this->new_ip.push(new NodeConnection("5.45.99.75", "8444", this));
	this->new_ip.push(new NodeConnection("75.167.159.54", "8444", this));
	this->new_ip.push(new NodeConnection("95.165.168.168", "8444", this));
	this->new_ip.push(new NodeConnection("85.180.139.241", "8444", this));
	this->new_ip.push(new NodeConnection("158.222.211.81", "8080", this));
	this->new_ip.push(new NodeConnection("178.62.12.187", "8448", this));
	this->new_ip.push(new NodeConnection("24.188.198.204", "8111", this));
	this->new_ip.push(new NodeConnection("109.147.204.113", "1195", this));
	this->new_ip.push(new NodeConnection("178.11.46.221", "8444", this));
	this->new_ip.push(new NodeConnection("79.215.196.101", "8444", this));
	this->new_ip.push(new NodeConnection("190.244.108.137", "8444", this));
	this->new_ip.push(new NodeConnection("198.244.103.16", "8445", this));
	this->new_ip.push(new NodeConnection("127.0.0.1", "8444", this));
	this->load("save");
}

BitMRC::~BitMRC()
{
	this->running = false;
	
	this->new_ip.push(new NodeConnection(this));
	this->new_packets.push(Packet());

	if (this->thread_new_ip.joinable())
		this->thread_new_ip.join();

	if (this->thread_new_hashes.joinable())
		this->thread_new_hashes.join();

	if (this->thread_new_packets.joinable())
		this->thread_new_packets.join();



	for (unsigned int i = 0; i < Nodes.size(); i++)
	{
		if (Nodes[i] != NULL)
			delete Nodes[i];
	}
	
	this->save("save");
	
	//important: clear thread first then clear data
	this->Nodes.clear();

	this->PubAddresses.clear(); 
	this->PrivAddresses.clear();
}

void BitMRC::start()
{
	this->running = true;
	this->thread_new_ip = thread(&BitMRC::listen_ips, this);
	this->thread_new_packets = thread(&BitMRC::listen_packets, this);
}

void BitMRC::listen_ips()
{
	while (this->running)
	{
		NodeConnection* tmp = this->new_ip.pop();
		this->Nodes.push_back(tmp);
		this->Nodes[this->Nodes.size() - 1]->Start();
	}
}

void BitMRC::listen_packets()
{
	while (this->running)
	{
		Packet packet = this->new_packets.pop();
		for (unsigned int i = 0; i < Nodes.size(); i++)
		{
			Nodes.operator[](i)->Packets.push(packet);
		}
	}
}

void BitMRC::getPubKey(PubAddr address)
{
	int find = false;
	unsigned int i = 0;
	while(i < this->PubAddresses.size() && !find)
	{
		if (this->PubAddresses[i].getVersion() == address.getVersion()) //check same version
		{
			if (address.getVersion() >= 4)
			{
				if (this->PubAddresses[i].getTag() == address.getTag())
				{
					if (!this->PubAddresses[i].waitingPubKey()) //check if we already have the pubkeys
						return; //we already have it!
					else
						find = true; //continue we need to request it
				}
			}
			else
			{
				if (this->PubAddresses[i].getRipe() == address.getRipe())
				{
					if (!this->PubAddresses[i].waitingPubKey()) //check if we already have the pubkeys
						return; // we already have it!
					else
						find = true; //continue we need to request it
				}
			}
		}
		i++;
	}
	if (!find)
	{
		for (int i = 0; i < this->sharedObj.Dim; i++)
		{
			hash_table<ustring>::linked_node * cur = this->sharedObj.Table[i];
			while (cur != NULL)
			{
				
				object obj;
				obj.message_payload = cur->info;
				obj.decodeData();
				if (obj.objectType == type_pubkey)
				{
					packet_pubkey pubkey(obj);
					if (pubkey.tag == address.getTag())
					{
						address.decodeFromObj(pubkey);

						this->saveAddr(address);
						return;
					}
				}

				cur = cur->next;
			}
		}
	}
	packet_getpubkey obj;
	obj.tag = address.getTag();
	
	obj.objectType = 0;
	obj.version = address.getVersion();
	obj.stream = address.getStream();
	obj.encodeObject();

	if(!find) //add the pubkey only if is not already present
		this->saveAddr(address);

	time_t ltime = std::time(nullptr);
	time_t TTL = 60 * 60;
	ltime = ltime + TTL;

	obj.Time = ltime;

	this->sendObj(obj);
}

void BitMRC::sendMessage(ustring message, PubAddr toAddr, Addr fromAddr)
{
	packet_msg packet;
	
	time_t ltime = std::time(nullptr);
	time_t TTL = 60 * 60;
	ltime = ltime + TTL;

	packet.Time = ltime;

	packet.objectType = 2;
	packet.Time = ltime;
	packet.stream = fromAddr.getStream();
	packet.version = 1;

	packet.encodePayload();

	ustring msg;
	//msg.appendVarInt_B(1); //msg version
	msg.appendVarInt_B(fromAddr.getVersion());
	msg.appendVarInt_B(fromAddr.getStream());
	msg.appendInt32(1);
	msg.append(fromAddr.getPubSigningKey().c_str() + 1,64);
	msg.append(fromAddr.getPubEncryptionKey().c_str() + 1, 64);
	if (fromAddr.getVersion() >= 3)
	{
		msg.appendVarInt_B(fromAddr.getNonce());
		msg.appendVarInt_B(fromAddr.getExtra());
	}
	msg += toAddr.getRipe();
	msg.appendVarInt_B(1); //message encoding 1: trivia no body or subject
	msg.appendVarInt_B(message.size());
	msg += message;
	msg.appendVarInt_B(1); //ack len 1 so it wont work just testing
	msg.appendInt8(1);

	OID CURVE = secp256k1();
	AutoSeededRandomPool prng;

	ECDSA<ECP, SHA1>::PrivateKey privateKey;

	Integer x;
	x.Decode(fromAddr.getPrivSigningKey().c_str(), fromAddr.getPrivSigningKey().size());
	privateKey.Initialize(CURVE, x);

	ECDSA<ECP, SHA1>::Signer signer(privateKey);

	
	string signature;
	string mess;
	ustring mess1;
	unsigned int i = 8;
	mess1.appendInt64(packet.message_payload.getInt64(i));
	mess1.appendInt32(packet.message_payload.getInt32(i));
	mess1.appendVarInt_B(packet.message_payload.getVarInt_B(i));
	mess1.appendVarInt_B(packet.message_payload.getVarInt_B(i));
	
	mess += mess1.toString();
	mess += msg.toString();

	StringSource ss(mess, true /*pump all*/,
		new SignerFilter(prng,
			signer,
			new StringSink(signature)
			) // SignerFilter
		); // StringSource

	//DER encoding
	Integer r, s;
	StringStore store(signature);
	r.Decode(store, signature.size() / 2);
	s.Decode(store, signature.size() / 2);
	string sign;
	StringSink sink(sign);
	DERSequenceEncoder seq(sink);
	r.DEREncode(seq);
	s.DEREncode(seq);
	seq.MessageEnd();
	//end conversion

	msg.appendVarInt_B(sign.size());
	msg.append((unsigned char*)sign.c_str(), sign.size());

	ustring encrypted = fromAddr.encode(toAddr.getPubEncryptionKey(), fromAddr.getPrivEncryptionKey(), fromAddr.getPubEncryptionKey(), msg);
	
	packet.objectPayload = encrypted;

	this->sendObj(packet);
}


/*
ECPPoint point;
i = 1;
ustring a;
a += fromAddr.getPubSigningKey();
string xA = a.getString(32, i);
string yA = a.getString(32, i);

string encoded;
StringSource((byte*)xA.c_str(), xA.size(), true,
new HexEncoder(
new StringSink(encoded)
) // HexEncoder
); // StringSource
cout << "Q.x: " << encoded << endl;

encoded.clear();
StringSource((byte*)yA.c_str(), yA.size(), true,
new HexEncoder(
new StringSink(encoded)
) // HexEncoder
); // StringSource
cout << "Q.y: " << encoded << endl;
point.identity = false;
point.x.Decode((byte*)xA.c_str(), 32);
point.y.Decode((byte*)yA.c_str(), 32);

ECDSA<ECP, SHA1>::PublicKey publicKey;
publicKey.Initialize(CURVE, point);
//privateKey.MakePublicKey(publicKey);

bool res = publicKey.Validate(prng, 3);
//const ECP::Point& qq = publicKey.GetPublicElement();
//cout << "Q.x: " << std::hex << qq.x << endl;
//cout << "Q.y: " << std::hex << qq.y << endl;

ECDSA<ECP, SHA1>::Verifier verifier(publicKey);

// Result of the verification process
bool result = false;


StringSource sss(signature + mess, true,
new SignatureVerificationFilter(
verifier,
new ArraySink((byte*)&result, sizeof(result))
) // SignatureVerificationFilter
);

if (result)
{
int a = 4;
}

*/


void BitMRC::sendObj(object obj)
{
	obj.nonce = 0;

	memset(obj.command, 0x00, sizeof obj.command);
	strncpy(obj.command, "object",7);

	obj.encodePayload();

	unsigned __int64 nonce = searchPow(obj.message_payload, obj.Time);

	obj.nonce = nonce;

	obj.encodePayload();

	obj.setChecksum_Lenght_Magic();
	unsigned int i = 20;
	int version = (int)obj.message_payload.getVarInt_B(i);
	int stream = (int)obj.message_payload.getVarInt_B(i);
	this->new_packets.push(obj);
	
	sTag tag;
	ustring hash = doubleHash(obj.message_payload);

	memcpy(tag.ch, hash.c_str(), 32);
}

bool BitMRC::decryptMsg(packet_msg msg)
{
	std::unique_lock<std::mutex> mlock(this->mutex_priv);
	ustring recovered;
	bool decryptionSuccess = false;
	unsigned int i = 0;
	while( i < this->PrivAddresses.size() && !decryptionSuccess)
	{
		try
		{
			recovered = this->PrivAddresses[i].decode(msg.objectPayload, this->PrivAddresses[i].getPrivEncryptionKey());
			decryptionSuccess = true;
		}catch(...)
		{
			return false;
		}
		i++;
	}
	i--;
	bool signatureSuccess = false;
	try {

		if (decryptionSuccess)
		{
			unsigned int j = 0;
			int version = (int)recovered.getVarInt_B(j);
			int stream = (int)recovered.getVarInt_B(j);
			int bitfield = recovered.getInt32(j);
			ustring pubSigningKey = recovered.getUstring(64, j);
			ustring pubEncryptionKey = recovered.getUstring(64, j);
			int Nonce = 1000;
			int Extra = 1000;
			if (version >= 3)
			{
				Nonce = (int)recovered.getVarInt_B(j);
				Extra = (int)recovered.getVarInt_B(j);
			}
			ustring Ripe = recovered.getUstring(20, j);
			int messageEncoding = (int)recovered.getVarInt_B(j);
			ustring Message = recovered.getVarUstring_B(j);
			ustring AckData = recovered.getVarUstring_B(j);
			int k = j;
			ustring sign = recovered.getVarUstring_B(j);

			//checking signature

			OID CURVE = secp256k1();
			AutoSeededRandomPool prng;

			ECPPoint point;
			i = 0;
			string xA = pubSigningKey.getString(32, i);
			string yA = pubSigningKey.getString(32, i);

			point.identity = false;
			point.x.Decode((byte*)xA.c_str(), 32);
			point.y.Decode((byte*)yA.c_str(), 32);

			ECDSA<ECP, SHA1>::PublicKey publicKey;
			publicKey.Initialize(CURVE, point);

			bool res = publicKey.Validate(prng, 3);

			ECDSA<ECP, SHA1>::Verifier verifier(publicKey);

			// Result of the verification process
			bool result = false;

			ustring mess1;
			j = 8;
			mess1.appendInt64(msg.message_payload.getInt64(j));
			mess1.appendInt32(msg.message_payload.getInt32(j));
			mess1.appendVarInt_B(msg.message_payload.getVarInt_B(j));
			mess1.appendVarInt_B(msg.message_payload.getVarInt_B(j));

			string mess;

			mess += mess1.toString();
			j = 0;
			mess += recovered.getString(k, j);

			string tmp_sign = sign.toString();

			//BER decoding
			Integer r, s;
			StringStore store(tmp_sign);
			BERSequenceDecoder seq(store);
			r.BERDecode(seq);
			s.BERDecode(seq);
			seq.MessageEnd();
			string signature;
			StringSink sink(signature);
			r.Encode(sink, 32);
			s.Encode(sink, 32);
			//end conversion

			StringSource sss(signature + mess, true,
				new SignatureVerificationFilter(
					verifier,
					new ArraySink((byte*)&result, sizeof(result))
					) // SignatureVerificationFilter
				);

			if (result)
			{
				printf("NEW MESSAGE: %s", Message.c_str());

				signatureSuccess = true;
			}
		}
	}
	catch (...)
	{
		return false;
	}
	mlock.unlock();
	return decryptionSuccess && signatureSuccess;
}

void BitMRC::generateDeterministicAddr(ustring passphrase, int n)
{
	int nonce = 0;
	int i = 0;
	while(i<n)
	{
		Addr address;

		int nonce_old = nonce;
		nonce = address.generateDeterministic(passphrase, nonce);

		if (nonce == nonce_old)
			continue;

		this->saveAddr(address);
		i++;
	}
}

void BitMRC::saveAddr(PubAddr address)
{
	std::unique_lock<std::mutex> mlock(this->mutex_pub);

	if (!address.getAddress().empty())
	{
		bool find = false;
		unsigned int i = 0;
		while (i < this->PubAddresses.size() && !find)
		{
			if (this->PubAddresses[i] == address)
				find = true;
			i++;
		}
		if (!find)
			this->PubAddresses.push_back(address);
	}

	mlock.unlock();
}

void BitMRC::saveAddr(Addr address)
{
	std::unique_lock<std::mutex> mlock(this->mutex_priv);

	if (!address.getAddress().empty())
	{
		bool find = false;
		unsigned int i = 0;
		while (i < this->PrivAddresses.size() && !find)
		{
			if (this->PrivAddresses[i] == address)
				find = true;
			i++;
		}
		if (!find)
			this->PrivAddresses.push_back(address);
	}

	mlock.unlock();
}





ustring BitMRC::inventoryHash(ustring data)
{
	ustring invHash;

	CryptoPP::SHA512 hash;
	byte digest[CryptoPP::SHA512::DIGESTSIZE];
	byte digest2[CryptoPP::SHA512::DIGESTSIZE];
	hash.CalculateDigest(digest, (byte*)data.c_str(), data.length());
	hash.CalculateDigest(digest2, digest, sizeof digest);

	invHash.append(digest2, 32);

	return invHash;
}

void BitMRC::load(string path)
{
	FILE * pFile = fopen(path.c_str(), "rb");
	FILE * kFile = fopen("keys.dat", "rb");
	if (!pFile)
		return;
	if (!kFile)
		return;

	file_ustring reader;
	reader.setFile(pFile);

	time_t ltime = std::time(nullptr);	
	try {
		while (!feof(pFile))
		{
			
			ustring tagS = reader.getUstring(32);
			ustring infoS = reader.getVarUstring();

			object obj;
			obj.message_payload = infoS;
			obj.decodeData();

			if (ltime < obj.Time)
				this->sharedObj.insert(infoS, tagS);
		}
	}
	catch (...)
	{
		//nothing just cant read any more blocks
	}
	reader.setFile(kFile);
	try {
		while (!feof(kFile))
		{
			Addr tmp;
			ustring Address = reader.getVarUstring();
			ustring privE = reader.getVarUstring();
			ustring privS = reader.getVarUstring();
			tmp.loadAddr(Address);
			if (!tmp.loadKeys(tmp.getPubOfPriv(privE), tmp.getPubOfPriv(privS), privE, privS, tmp.getStream(), tmp.getVersion())) //todo create a simpler function
				continue;
			this->saveAddr(tmp);
		}
	}
	catch (...)
	{
		//nothing just end of file or nothing can do
	}
	fclose(pFile);
}

void BitMRC::save(string path)
{
	FILE * pFile = fopen(path.c_str(), "wb");
	FILE * kFile = fopen("keys.dat", "wb");
	
	if (!pFile)
		return;
	if (!kFile)
		return;
	try {
		file_ustring writer;
		writer.setFile(pFile);
		for (int i = 0; i < this->sharedObj.Dim; i++)
		{
			hash_table<ustring>::linked_node * cur = this->sharedObj.Table[i];
			while (cur != NULL)
			{
				writer.writeUstring(cur->hash);
				writer.writeVarUstring(cur->info);
				cur = cur->next;
			}
		}
		writer.setFile(kFile);
		for (unsigned int i = 0; i < this->PrivAddresses.size(); i++)
		{
			writer.writeVarUstring(this->PrivAddresses[i].getAddress());
			writer.writeVarUstring(this->PrivAddresses[i].getPrivEncryptionKey());
			writer.writeVarUstring(this->PrivAddresses[i].getPrivSigningKey());
		}
	}
	catch (...)
	{
		//nothing can do
	}
	fclose(pFile);
	fclose(kFile);
}