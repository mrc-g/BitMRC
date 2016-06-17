#include <BitMRC.h>
#ifdef LINUX
#include <string.h> /* memset */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
#include <NodeBlacklist.h>

BitMRC::BitMRC()
{
	this->load("save");
}

void BitMRC::init()
{
	int dns = 2;

	addrinfo *result = NULL, hints;
	sockaddr_in  *sockaddr_ipv4;

#ifndef LINUX
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif


	ZeroMemory(&hints, sizeof(hints));

	NodeBlacklist bl;

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	if (getaddrinfo("bootstrap8444.bitmessage.org", "0", &hints, &result))
	{
		dns--;
	}
	else
	{
		for (addrinfo *ptr = result; ptr != NULL; ptr = ptr->ai_next) {
			if (ptr->ai_family == AF_INET) {
				if (bl.is_blacklisted(ptr, AF_INET) == 0) {
					sockaddr_ipv4 = (sockaddr_in *)ptr->ai_addr;
					this->connectNode(new NodeConnection(inet_ntoa(sockaddr_ipv4->sin_addr), "8444", this));
				}
			}
		}
		freeaddrinfo(result);
	}


	if (getaddrinfo("bootstrap8080.bitmessage.org", "0", &hints, &result))
	{
		dns--;
	}
	else
	{
		for (addrinfo *ptr = result; ptr != NULL; ptr = ptr->ai_next)
		{
			if (ptr->ai_family == AF_INET)
			{
				sockaddr_ipv4 = (sockaddr_in *)ptr->ai_addr;
				if (bl.is_blacklisted(ptr, AF_INET) == 0) {
					sockaddr_ipv4 = (sockaddr_in *)ptr->ai_addr;
					this->connectNode(new NodeConnection(inet_ntoa(sockaddr_ipv4->sin_addr), "8080", this));
				}
			}
		}

		freeaddrinfo(result);
	}
#ifndef LINUX
	WSACleanup();
#endif

	if (1)//for now i leave this active, I noticed that sometimes bootstrap dns work but no ip is working.
	{
		//printf("Node bootstrapping failed!\n");
		//trying hardcoded nodes
		//TODO save a list of nodes on disk
		this->connectNode(new NodeConnection("5.45.99.75", "8444", this));
		this->connectNode(new NodeConnection("75.167.159.54", "8444", this));
		this->connectNode(new NodeConnection("95.165.168.168", "8444", this));
		this->connectNode(new NodeConnection("85.180.139.241", "8444", this));
		this->connectNode(new NodeConnection("158.222.211.81", "8080", this));
		this->connectNode(new NodeConnection("178.62.12.187", "8448", this));
		this->connectNode(new NodeConnection("24.188.198.204", "8111", this));
		this->connectNode(new NodeConnection("109.147.204.113", "1195", this));
		this->connectNode(new NodeConnection("178.11.46.221", "8444", this));
		this->connectNode(new NodeConnection("79.215.196.101", "8444", this));
		this->connectNode(new NodeConnection("190.244.108.137", "8444", this));
		this->connectNode(new NodeConnection("198.244.103.16", "8445", this));
		this->connectNode(new NodeConnection("127.0.0.1", "8444", this));
	}
}

BitMRC::~BitMRC()
{
	this->running = false;
	
	std::unique_lock<std::shared_timed_mutex> mlock(this->mutex_nodes);
	for (unsigned int i = 0; i < Nodes.size(); i++)
		Nodes[i]->Close();

	for (unsigned int i = 0; i < Nodes.size(); i++)
	{
		if (Nodes[i] != NULL)
			delete Nodes[i];
	}

	//important: clear thread first then clear data
	this->Nodes.clear();

	mlock.unlock();

	this->new_packets.push(Packet());
	
	this->new_inv.push(sTag());

	if (this->thread_new_packets.joinable())
		this->thread_new_packets.join();

	if (this->thread_new_inv.joinable())
		this->thread_new_inv.join();
	
	if (this->thread_init.joinable())
		this->thread_init.join();

	this->save("save");

	std::unique_lock<std::shared_timed_mutex> mlock1(this->mutex_priv);
	std::unique_lock<std::shared_timed_mutex> mlock2(this->mutex_pub);

	this->PubAddresses.clear(); 
	this->PrivAddresses.clear();
}

void BitMRC::start()
{
	this->running = true;
	this->thread_new_packets = thread(&BitMRC::listen_packets, this);
	this->thread_new_inv = thread(&BitMRC::listen_inv, this);
	this->thread_init = thread(&BitMRC::init, this);
}

void BitMRC::processObj(object obj)
{
	bool check = checkPow(obj.message_payload, obj.Time);

	//if not ignore
	if (check)
	{
		ustring invHash = this->inventoryHash(obj.message_payload);
		int present = this->sharedObj.insert(obj.message_payload, invHash);
		if (present == 2)
			return; //exit because this object is already analized
		sTag tag;
		memcpy(tag.ch, invHash.c_str(), 32);

		this->new_inv.push(tag);

		if (obj.objectType == type_getpubkey)
		{
			packet_getpubkey getpubkey(obj);

			std::shared_lock<std::shared_timed_mutex> mlock(this->mutex_priv);

			for (unsigned int i = 0; i < this->PrivAddresses.size(); i++)
			{
				ustring tag = this->PrivAddresses[i].getTag();
				if (getpubkey.tag == tag)
				{
					if (this->PrivAddresses[i].getLastPubKeyRequest() + 60 * 60 * 24 * 4 < time(NULL))
						this->sendObj(this->PrivAddresses[i].encodePubKey());
					//else
					//printf("PubKey already shared recently");
				}
			}

			mlock.unlock();
		}
		else if (obj.objectType == type_pubkey)
		{
			packet_pubkey pubkey(obj);

			std::shared_lock<std::shared_timed_mutex> mlock(this->mutex_pub);

			for (unsigned int i = 0; i < this->PubAddresses.size(); i++)
			{
				if (!this->PubAddresses[i].waitingPubKey())
					continue;
				ustring tag = this->PubAddresses[i].getTag();
				if (pubkey.tag == tag)
				{
					this->PubAddresses[i].decodeFromObj(pubkey);
				}
			}

			mlock.unlock();
		}
		else if (obj.objectType == type_msg)
		{
			packet_msg msg(obj);

			if (this->decryptMsg(msg)) //it takes like 1-4 milliseconds
			{
				//printf("Message accepted\n");
			}
		}
		else if (obj.objectType == type_broadcast)
		{
			packet_broadcast broadcast(obj);

			if (this->decryptMsg(broadcast))
			{
				//printf("broadcast decrypted\n");
			}
		}
	}
}

void BitMRC::connectNode(NodeConnection *node)
{
	if (node != NULL)
	{
		std::unique_lock<std::shared_timed_mutex> mlock(this->mutex_nodes);
		this->Nodes.push_back(node);
		this->Nodes[this->Nodes.size() - 1]->Start();
		mlock.unlock();
	}
}

void BitMRC::listen_packets()
{
	while (this->running)
	{
		Packet packet = this->new_packets.pop();
		std::shared_lock<std::shared_timed_mutex> mlock(this->mutex_nodes);
		for (unsigned int i = 0; i < Nodes.size(); i++)
		{
			Nodes.operator[](i)->Packets.push(packet);
		}
		mlock.unlock();
	}
}

void BitMRC::listen_inv()
{
	while (this->running)
	{
		if (this->new_inv.size() < 1000)
		{
			std::uniform_int_distribution<int> distribution(-1000, 1000);
			std::mt19937 engine;
			std::random_device rd;
			engine.seed(rd());
			int random = distribution(engine);
			SLEEP(5000 + random);//sleep 5 +-1 sec
		}
		packet_inv inv;

		while (this->new_inv.size() > 0)
			inv.inventory.push_back(this->new_inv.pop());

		inv.encodeData();

		if (inv.inventory.size() > 0)
			this->send(inv);
	}
}

void BitMRC::getPubKey(PubAddr address)
{
	int find = false;
	unsigned int i = 0;
	while (i < this->PubAddresses.size() && !find)
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
					{
						this->PubAddresses[i].setLastPubKeyRequest(time(NULL)); //we are asking it right now
						find = true; //continue we need to request it
					}
				}
			}
			else
			{
				if (this->PubAddresses[i].getRipe() == address.getRipe())
				{
					if (!this->PubAddresses[i].waitingPubKey()) //check if we already have the pubkeys
						return; // we already have it!
					else
					{
						this->PubAddresses[i].setLastPubKeyRequest(time(NULL)); //we are asking it right now
						find = true; //continue we need to request it
					}
				}
			}
		}
		i++;
	}
	if (!find)
	{
		for (int i = 0; i < this->sharedObj.Dim; i++)
		{
			std::shared_lock<std::shared_timed_mutex> mlock(this->sharedObj.mutex_);
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

						this->addAddr(address);
						return;
					}
				}

				cur = cur->next;
			}
			mlock.unlock();
		}
	}
	packet_getpubkey obj;
	obj.tag = address.getTag();

	obj.objectType = 0;
	obj.version = address.getVersion();
	obj.stream = address.getStream();
	obj.encodeObject();

	if (!find) //add the pubkey only if is not already present
	{
		address.setLastPubKeyRequest(time(NULL));
		this->addAddr(address);
	}
	time_t ltime = std::time(nullptr);
	std::uniform_int_distribution<int> distribution(-300, 300);
	std::mt19937 engine;
	std::random_device rd;
	engine.seed(rd());
	int random = distribution(engine);
	time_t TTL = 4 * 24 * 60 * 60 + random; //4 days +- 5 min
	ltime = ltime + TTL;

	obj.Time = ltime;

	this->sendObj(obj);
}

void BitMRC::sendMessage(ustring message, PubAddr toAddr, Addr fromAddr)
{
	//TODO: check fromAddr is not empty
	if (toAddr.waitingPubKey())
	{
		this->getPubKey(toAddr); //TODO: make a system for key asked recently
		
		// Find the address in our pool, as the one we have is only a copy
		PubAddr* target = nullptr;
		for (auto i = this->PubAddresses.begin(); i != this->PubAddresses.end(); ++i)
		{
			if (*i == toAddr)
			{
				target = &(*i);
				break; // We found our pointer, stop the loop
			}
		}

		// If this fails, we won't be able to send the message.
		// The toAddr we passed is only a copy, not a reference,
		// therefore it will never contain the public key, even if it has been added to the pool.
		// Exiting here is therefore the only possible option.
		// Although this should never happen, as 'this->getPubKey(toAddr)' should insert it.
		if (!target)
		{
			return;
		}

		while (target->waitingPubKey()) //TODO: make a list of waiting message for their pubkey
			SLEEP(10000); //just stuck there if no pubkey is going to appear

		// Copy the acquired data over to our object
		toAddr = *target;
	}
	packet_msg packet;
	
	time_t ltime = std::time(nullptr);
	std::uniform_int_distribution<int> distribution(-300, 300);
	std::mt19937 engine;
	std::random_device rd;
	engine.seed(rd());
	int random = distribution(engine);
	time_t TTL = 4 * 24 * 60 * 60 + random; //4 days +- 5 min
	ltime = ltime + TTL;

	packet.Time = ltime;

	packet.objectType = type_msg;
	packet.Time = ltime;
	packet.stream = fromAddr.getStream();
	packet.version = 1;

	packet.encodePayload();

	OID CURVE = secp256k1();
	AutoSeededRandomPool rng;

	ECDH < ECP >::Domain dhA(CURVE);
	//generating ephemeral key pair
	SecByteBlock privA(dhA.PrivateKeyLength()), pubA(dhA.PublicKeyLength());

	dhA.GenerateKeyPair(rng, privA, pubA);

	ustring pubEKey;
	pubEKey.append(pubA.data(), pubA.size());

	ustring privEKey;
	privEKey.append(privA.data(), privA.size());

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

	packet.objectPayload = fromAddr.encode(toAddr.getPubEncryptionKey(), privEKey, pubEKey, msg);
	

	this->sendObj(packet);
}

void BitMRC::sendBroadcast(ustring message, Addr address)
{
	//TODO: check address is not empty
	ustring pubE = address.getPubOfPriv(address.getTagE());

	packet_broadcast packet;
	
	time_t ltime = std::time(nullptr);
	std::uniform_int_distribution<int> distribution(-300, 300);
	std::mt19937 engine;
	std::random_device rd;
	engine.seed(rd());
	int random = distribution(engine);
	time_t TTL = 4 * 24 * 60 * 60 + random; //4 days +- 5 min
	ltime = ltime + TTL;

	packet.Time = ltime;

	packet.objectType = type_broadcast;
	packet.Time = ltime;
	packet.stream = address.getStream();
	packet.version = 5; //broadcast version 5: included tag

	packet.encodePayload();

	OID CURVE = secp256k1();
	AutoSeededRandomPool rng;

	ECDH < ECP >::Domain dhA(CURVE);
	//generating ephemeral key pair
	SecByteBlock privA(dhA.PrivateKeyLength()), pubA(dhA.PublicKeyLength());

	dhA.GenerateKeyPair(rng, privA, pubA);

	ustring pubEKey;
	pubEKey.append(pubA.data(), pubA.size());

	ustring privEKey;
	privEKey.append(privA.data(), privA.size());

	ustring msg;
	//msg.appendVarInt_B(1); //msg version
	msg.appendVarInt_B(address.getVersion());
	msg.appendVarInt_B(address.getStream());
	msg.appendInt32(1);
	msg.append(address.getPubSigningKey().c_str() + 1, 64);
	msg.append(address.getPubEncryptionKey().c_str() + 1, 64);
	if (address.getVersion() >= 3)
	{
		msg.appendVarInt_B(address.getNonce());
		msg.appendVarInt_B(address.getExtra());
	}
	msg.appendVarInt_B(1); //message encoding 1: trivia no body or subject
	msg.appendVarInt_B(message.size());
	msg += message;

	AutoSeededRandomPool prng;

	ECDSA<ECP, SHA1>::PrivateKey privateKey;

	Integer x;
	x.Decode(address.getPrivSigningKey().c_str(), address.getPrivSigningKey().size());
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
	if (packet.version == 5)
	{
		packet.tag = address.getTag();
		mess1 += packet.tag;
	}

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

	packet.encrypted = address.encode(pubE, privEKey, pubEKey, msg);

	packet.encodeObject();

	this->sendObj(packet);
}

void BitMRC::sendObj(object obj)
{
	obj.nonce = 0;

	memset(obj.command, 0x00, sizeof obj.command);
	strncpy(obj.command, "object",7);

	obj.encodePayload(); //time should be already 

	unsigned __int64 nonce = searchPow(obj.message_payload, obj.Time);

	obj.nonce = nonce;

	obj.encodePayload();

	this->propagate(obj);
}

void BitMRC::propagate(object obj)
{
	if (obj.message_payload.length() > UINT64_C(0x40000)) //2^16
		return;

	if (!checkPow(obj.message_payload, obj.Time))
		return;

	time_t ltime = std::time(nullptr);

	if (obj.Time - ltime > 28 * 24 * 60 * 60 + 10800)
		return;

	if (obj.Time - ltime < -3600)
		return;

	ustring invHash = this->inventoryHash(obj.message_payload);
	int present = this->sharedObj.insert(obj.message_payload, invHash);

	sTag tag;
	memcpy(tag.ch, invHash.c_str(), 32);

	this->new_inv.push(tag);
}

void BitMRC::send(Packet packet)
{
	packet.setChecksum_Lenght_Magic();
	this->new_packets.push(packet);
}

bool BitMRC::decryptMsg(packet_msg msg)
{
	std::shared_lock<std::shared_timed_mutex> mlock(this->mutex_priv);
	ustring recovered;
	bool decryptionSuccess = false;
	unsigned int address = 0;
	while( address < this->PrivAddresses.size() && !decryptionSuccess)
	{
		try
		{
			recovered = this->PrivAddresses[address].decode(msg.objectPayload, this->PrivAddresses[address].getPrivEncryptionKey());
			decryptionSuccess = true;
		}catch(...)
		{
			//nothing just failed with this address
		}
		address++;
	}
	address--;
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
			ustring destRipe = recovered.getUstring(20, j);
			int messageEncoding = (int)recovered.getVarInt_B(j);
			ustring Message = recovered.getVarUstring_B(j);
			ustring AckData = recovered.getVarUstring_B(j);
			int k = j;
			ustring sign = recovered.getVarUstring_B(j);

			if (destRipe != this->PrivAddresses[address].getRipe())
			{
				//printf("Preventing a Surreptitious Forwarding Attack.");
				return false;
			}

			//checking signature

			OID CURVE = secp256k1();
			AutoSeededRandomPool prng;

			ECPPoint point;
			unsigned int i = 0;
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
				//propagate ack
				Packet ack(AckData);
				this->propagate(ack);

				//storing it
				BitMRC::message Mess;

				Mess.to = this->PrivAddresses[address].getAddress();
				
				PubAddr from;
				ustring addr = from.buildAddressFromKeys(pubSigningKey, pubEncryptionKey, stream, version);

				from.loadAddr(addr);
				from.loadKeys(pubSigningKey, pubEncryptionKey, Nonce, Extra);
				
				this->addAddr(from); //saving it

				Mess.from = addr;
				Mess.info = Message;

				Mess.signature = sign;

				int ret = this->messages.insert(Mess, sign);

				if (ret)
					this->new_messages.push(Mess);	//add to new incoming messages if not already there

				signatureSuccess = true;
			}
		}
	}
	catch (...)
	{
		return false;
	}
	return decryptionSuccess && signatureSuccess;
}

bool BitMRC::decryptMsg(packet_broadcast msg)
{
	std::shared_lock<std::shared_timed_mutex> mlock(this->mutex_subs);
	ustring recovered;
	bool decryptionSuccess = false;
	unsigned int address = 0;
	while (address < this->Subscriptions.size() && !decryptionSuccess)
	{
		if (msg.version == 4)
		{
			try
			{
				recovered = this->Subscriptions[address].decode(msg.encrypted, this->Subscriptions[address].getTagE());
				decryptionSuccess = true;
			}
			catch (...)
			{
				//nothing just failed with this address
			}
		}
		else if (msg.version == 5)
		{
			if (msg.tag == this->Subscriptions[address].getTag())
			{
				try
				{
					recovered = this->Subscriptions[address].decode(msg.encrypted, this->Subscriptions[address].getTagE());
					decryptionSuccess = true;
				}
				catch (...)
				{
					return false;
				}
			}
		}
		address++;
	}
	address--;
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
			int messageEncoding = (int)recovered.getVarInt_B(j);
			ustring Message = recovered.getVarUstring_B(j);
			int k = j;
			ustring sign = recovered.getVarUstring_B(j);

			//checking signature

			OID CURVE = secp256k1();
			AutoSeededRandomPool prng;

			ECPPoint point;
			unsigned int i = 0;
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
			if (msg.version == 5)
				mess1 += msg.tag;
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
				//storing it
				BitMRC::message Mess;

				Mess.to.fromString(string("Broadcast"));

				PubAddr from;
				ustring addr = from.buildAddressFromKeys(pubSigningKey, pubEncryptionKey, stream, version);

				from.loadAddr(addr);
				from.loadKeys(pubSigningKey, pubEncryptionKey, Nonce, Extra);

				this->addAddr(from); //saving it

				Mess.from = addr;
				Mess.info = Message;

				Mess.signature = sign;

				int ret = this->messages.insert(Mess, sign);

				if (ret)
					this->new_messages.push(Mess);	//add to new incoming messages if not already there

				signatureSuccess = true;
			}
		}
	}
	catch (...)
	{
		return false;
	}
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

		this->addAddr(address);
		i++;
	}
}

void BitMRC::addAddr(PubAddr address)
{
	std::unique_lock<std::shared_timed_mutex> mlock(this->mutex_pub);

	if (!address.getAddress().empty())
	{
		bool find = false;
		unsigned int i = 0;
		while (i < this->PubAddresses.size() && !find)
		{
			if (this->PubAddresses[i] == address)
			{
				if (!address.waitingPubKey() && this->PubAddresses[i].waitingPubKey())
					this->PubAddresses[i] = address;
				find = true;
			}
			i++;
		}
		if (!find)
			this->PubAddresses.push_back(address);
	}

	mlock.unlock();
}

void BitMRC::addAddr(Addr address)
{
	std::unique_lock<std::shared_timed_mutex> mlock(this->mutex_priv);

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

void BitMRC::addSubscription(PubAddr address)
{
	std::unique_lock<std::shared_timed_mutex> mlock(this->mutex_subs);

	if (!address.getAddress().empty())
	{
		bool find = false;
		unsigned int i = 0;
		while (i < this->Subscriptions.size() && !find)
		{
			if (this->Subscriptions[i] == address)
				find = true;
			i++;
		}
		if (!find)
			this->Subscriptions.push_back(address);
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

	file_ustring reader;
	if (kFile)
	{
		reader.setFile(kFile);
		try {
			while (!feof(kFile))
			{
				Addr tmp;
				int type = reader.getInt8();
				if (type == 0)
				{
					ustring address = reader.getVarUstring_B();
					ustring privE = reader.getVarUstring_B();
					ustring privS = reader.getVarUstring_B();
					time_t lastPubKey = reader.getInt64();
					tmp.loadAddr(address);
					if (!tmp.loadKeys(tmp.getPubOfPriv(privE), tmp.getPubOfPriv(privS), privE, privS, tmp.getStream(), tmp.getVersion())) //todo create a simpler function
						continue;

					tmp.setLastPubKeyRequest(lastPubKey);

					this->addAddr(tmp);
				}
				else if (type == 1)
				{
					PubAddr tmp;

					bool waiting = reader.getInt8() != 0;
					ustring address = reader.getVarUstring_B();
					tmp.loadAddr(address);
					if (!waiting)
					{
						ustring pubE = reader.getVarUstring_B();
						ustring pubS = reader.getVarUstring_B();
						int nonce = (int)reader.getVarInt_B();
						int extra = (int)reader.getVarInt_B();

						tmp.loadKeys(pubS, pubE, nonce, extra);
					}
					this->addAddr(tmp);
				}
			}
		}
		catch (...)
		{
			//nothing just end of file or nothing can do
		}
		fclose(kFile);
	}
	if (pFile)
	{
		reader.setFile(pFile);
		time_t ltime = std::time(nullptr);
		try {
			while (!feof(pFile))
			{

				ustring tagS = reader.getUstring(32);
				ustring infoS = reader.getVarUstring_B();

				object obj;
				obj.message_payload = infoS;

				obj.decodeData();

				if (ltime < obj.Time)
				{
					//processObj(obj); //process this message would take too much time (instant vs 30 sec)
					this->sharedObj.insert(infoS, tagS);
				}
			}
		}
		catch (...)
		{
			//nothing just cant read any more blocks
			//or currupted obj
		}
		fclose(pFile);
	}
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
		std::shared_lock<std::shared_timed_mutex> mlock(this->sharedObj.mutex_);
		for (int i = 0; i < this->sharedObj.Dim; i++)
		{
			hash_table<ustring>::linked_node * cur = this->sharedObj.Table[i];
			while (cur != NULL)
			{
				if (cur->hash.size() != 32 || cur->info.size()==0)//preventing currupted obj to be written
				{
					cur = cur->next;
					continue;
				}
				writer.writeUstring(cur->hash);
				writer.writeVarUstring_B(cur->info);
				cur = cur->next;
			}
		}
		mlock.unlock();

		writer.setFile(kFile);
		mlock = std::shared_lock<std::shared_timed_mutex>(this->mutex_priv);
		for (unsigned int i = 0; i < this->PrivAddresses.size(); i++)
		{
			writer.writeByte(0);//0 for private key
			writer.writeVarUstring_B(this->PrivAddresses[i].getAddress());
			writer.writeVarUstring_B(this->PrivAddresses[i].getPrivEncryptionKey());
			writer.writeVarUstring_B(this->PrivAddresses[i].getPrivSigningKey());
			writer.writeInt64(this->PrivAddresses[i].getLastPubKeyRequest());
		}
		mlock.unlock();

		mlock = std::shared_lock<std::shared_timed_mutex>(this->mutex_pub);
		for (unsigned int i = 0; i < this->PubAddresses.size(); i++)
		{
			writer.writeByte(1);//1 for public key
			writer.writeByte(this->PubAddresses[i].waitingPubKey()); //false if have keys
			writer.writeVarUstring_B(this->PubAddresses[i].getAddress());
			if (!this->PubAddresses[i].waitingPubKey())
			{
				writer.writeVarUstring_B(this->PubAddresses[i].getPubEncryptionKey());
				writer.writeVarUstring_B(this->PubAddresses[i].getPubSigningKey());
				writer.writeVarInt_B(this->PubAddresses[i].getNonce());
				writer.writeVarInt_B(this->PubAddresses[i].getExtra());
			}
		}
		mlock.unlock();

	}
	catch (...)
	{
		//nothing can do
	}
	fclose(pFile);
	fclose(kFile);
}
