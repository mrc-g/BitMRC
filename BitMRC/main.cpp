#include "main.h"

#include <cryptlib.h>
#include <sha.h>
#include <eccrypto.h>

using namespace CryptoPP::ASN1;
using namespace CryptoPP;

constexpr char hexmap[] = { '0', '1', '2', '3', '4', '5', '6', '7',
'8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };

std::string hexStr(unsigned char *data, int len)
{
	std::string s(len * 2, ' ');
	for (int i = 0; i < len; ++i) {
		s[2 * i] = hexmap[(data[i] & 0xF0) >> 4];
		s[2 * i + 1] = hexmap[data[i] & 0x0F];
	}
	return s;
}

int main()
{
	/*
	byte pubkey[] = { 0x04, 0x09, 0xd4, 0xe5, 0xc0, 0xab, 0x3d, 0x25,  0xfe, 0x04, 0x8c, 0x64, 0xc9, 0xda, 0x1a, 0x24,
					  0x2c, 0x7f, 0x19, 0x41, 0x7e, 0x95, 0x17, 0xcd,  0x26, 0x69, 0x50, 0xd7, 0x2c, 0x75, 0x57, 0x13,
					  0x58, 0x5c, 0x61, 0x78, 0xe9, 0x7f, 0xe0, 0x92,  0xfc, 0x89, 0x7c, 0x9a, 0x1f, 0x17, 0x20, 0xd5,
					  0x77, 0x0a, 0xe8, 0xea, 0xad, 0x2f, 0xa8, 0xfc,  0xbd, 0x08, 0xe9, 0x32, 0x4a, 0x5d, 0xde, 0x18,
					  0x57 };

	byte pubKA[] = {  0x04, 0x02, 0x93, 0x21, 0x3d, 0xcf, 0x13, 0x88,  0xb6, 0x1c, 0x2a, 0xe5, 0xcf, 0x80, 0xfe, 0xe6,
					  0xff, 0xff, 0xc0, 0x49, 0xa2, 0xf9, 0xfe, 0x73,  0x65, 0xfe, 0x38, 0x67, 0x81, 0x3c, 0xa8, 0x12,
					  0x92, 0xdf, 0x94, 0x68, 0x6c, 0x6a, 0xfb, 0x56,  0x5a, 0xc6, 0x14, 0x9b, 0x15, 0x3d, 0x61, 0xb3, 
					  0xb2, 0x87, 0xee, 0x2c, 0x7f, 0x99, 0x7c, 0x14,  0x23, 0x87, 0x96, 0xc1, 0x2b, 0x43, 0xa3, 0x86, 
			    	  0x5a };

	byte privKA[] = { 0x5b, 0xe6, 0xfa, 0xcd, 0x94, 0x1b, 0x76, 0xe9,  0xd3, 0xea, 0xd0, 0x30, 0x29, 0xfb, 0xdb, 0x6b, 
					  0x6e, 0x08, 0x09, 0x29, 0x3f, 0x7f, 0xb1, 0x97,  0xd0, 0xc5, 0x1f, 0x84, 0xe9, 0x6b, 0x8b, 0xa4 };

	byte IV[] = {	  0xbd, 0xdb, 0x7c, 0x28, 0x29, 0xb0, 0x80, 0x38,  0x75, 0x30, 0x84, 0xa2, 0xf3, 0x99, 0x16, 0x81 };

	byte message[] = {0x54, 0x68, 0x65, 0x20, 0x71, 0x75, 0x69, 0x63,  0x6b, 0x20, 0x62, 0x72, 0x6f, 0x77, 0x6e, 0x20,
					  0x66, 0x6f, 0x78, 0x20, 0x6a, 0x75, 0x6d, 0x70,  0x73, 0x20, 0x6f, 0x76, 0x65, 0x72, 0x20, 0x74,
					  0x68, 0x65, 0x20, 0x6c, 0x61, 0x7a, 0x79, 0x20,  0x64, 0x6f, 0x67, 0x2e, 0x04, 0x04, 0x04, 0x04 };

	OID CURVE = secp256k1();
	AutoSeededRandomPool rng;

	ECDH < ECP >::Domain dhA(CURVE), dhB(CURVE);
	SecByteBlock privA(privKA,dhA.PrivateKeyLength()), pubA(pubKA,dhA.PublicKeyLength());
	SecByteBlock privB(dhB.PrivateKeyLength()), pubB(pubkey,dhB.PublicKeyLength());



	Integer x;
	x.Decode(privKA, 32);

	ECIES<ECP>::Decryptor d0;

	d0.AccessKey().AccessGroupParameters().Initialize(CURVE);

	//Integer x(rng, Integer::One(), d0.AccessKey().GetGroupParameters().GetSubgroupOrder() - 1);
	d0.AccessKey().SetPrivateExponent(x);
	
	ECIES<ECP>::Encryptor e0(d0);

	string message2("ciaociao");

	string em0; // encrypted message
	StringSource ss1(message2, true, new PK_EncryptorFilter(rng, e0, new StringSink(em0)));
	string dm0; // decrypted message
	StringSource ss2(em0, true, new PK_DecryptorFilter(rng, d0, new StringSink(dm0)));

	cout << dm0 << endl;




	//dhA.GenerateKeyPair(rng, privA, pubA);

	if (dhA.AgreedValueLength() != dhB.AgreedValueLength())
		throw runtime_error("Shared shared size mismatch");

	SecByteBlock sharedA(dhA.AgreedValueLength()), sharedB(dhB.AgreedValueLength());

	if (!dhA.Agree(sharedA, privA, pubB))
		throw runtime_error("Failed to reach shared secret (A)");


	Integer ssa, ssb;

	ssa.Decode(sharedA.BytePtr(), sharedA.SizeInBytes());
	cout << "(A): " << std::hex << ssa << endl;

	cout << "Agreed to shared secret" << endl;

	uint8_t H[CryptoPP::SHA512::DIGESTSIZE];
	CryptoPP::SHA512 hash;

	hash.CalculateDigest(H, sharedA.BytePtr(), sharedA.SizeInBytes());



	AutoSeededRandomPool prng;

	byte key[32];

	memcpy(key, H, sizeof(key));
	//prng.GenerateBlock(key, sizeof(key));

	byte iv[16];

	memcpy(iv, IV, sizeof(IV));
	//prng.GenerateBlock(iv, sizeof(iv));

	string plain = "The quick brown fox jumps over the lazy dog.";
	string cipher, encoded, recovered;


	// Pretty print key
	encoded.clear();
	StringSource(key, sizeof(key), true,
		new HexEncoder(
			new StringSink(encoded)
			) // HexEncoder
		); // StringSource
	cout << "key: " << encoded << endl;

	// Pretty print iv
	encoded.clear();
	StringSource(iv, sizeof(iv), true,
		new HexEncoder(
			new StringSink(encoded)
			) // HexEncoder
		); // StringSource
	cout << "iv: " << encoded << endl;

	try
	{
		cout << "plain text: " << plain << endl;

		CBC_Mode< AES >::Encryption e;
		e.SetKeyWithIV(key, sizeof(key), iv);

		// The StreamTransformationFilter removes
		//  padding as required.
		StringSource s(plain, true,
			new StreamTransformationFilter(e,
				new StringSink(cipher)
				) // StreamTransformationFilter
			); // StringSource
	}
	catch (const CryptoPP::Exception& e)
	{
		cerr << e.what() << endl;
		exit(1);
	}


	// Pretty print
	encoded.clear();
	StringSource(cipher, true,
		new HexEncoder(
			new StringSink(encoded)
			) // HexEncoder
		); // StringSource
	cout << "cipher text: " << encoded << endl;


	try
	{
		CBC_Mode< AES >::Decryption d;
		d.SetKeyWithIV(key, sizeof(key), iv);

		// The StreamTransformationFilter removes
		//  padding as required.
		StringSource s(cipher, true,
			new StreamTransformationFilter(d,
				new StringSink(recovered)
				) // StreamTransformationFilter
			); // StringSource
		cout << "recovered text: " << recovered << endl;
	}
	catch (const CryptoPP::Exception& e)
	{
		cerr << e.what() << endl;
		exit(1);
	}

	*/


	//char * test = "2cU45DzGpa838WDYfYUfhqDXjVU7LqjEBY";
	//char * ret = new char[10];
	//size_t size=10;
	//b58enc(ret, &size, test, 3);

	//for (unsigned int i = 0; i<size; i++)
		//printf("%c", ret[i]);

	//char retu[1];
	//size_t size2 = 50;
	//if (b58tobin(retu, &size2, test) != true) {
	//	return 0;
	//}

	

	//string retur = hexStr((unsigned char *)retu, 34);

	//for (unsigned int i = 0; i < size2*2; i++)
	//{
	//	printf("%c", retur[i]);
	//}
	ustring addre;
	//addre.fromString(string("BM-2cXj1WaoHR6LKB5gLtdNsFhL57fPYm1BhA"));
	//addre.fromString(string("BM-2cU45DzGpa838WDYfYUfhqDXjVU7LqjEBY"));
	addre.fromString(string("BM-2cV1F1Ae2V5DtvTYhnNw81gvTyttH2SBYx"));
	PubAddr address;
	bool loaded = address.loadAddr(addre);

	//std::ostrstream oss;
	//oss << std::ios::hex << dec;
	//string ret(oss.str());

	//init endian for future conversion
	struct sockaddr_in6 sa;
	char str[INET6_ADDRSTRLEN];

	// store this IP address in sa:
	inet_pton(AF_INET6, "0000:0000:0000:0000:0000:ffff:5fa5:a8a8%25", &(sa.sin6_addr));

	// now get it back and print it
	inet_ntop(AF_INET6, &(sa.sin6_addr), str, INET6_ADDRSTRLEN);

	//printf("%s", str); // prints "2001:db8:8714:3a90::12"
	string ip = string(str);

	ustring ciao;
	ciao.fromString(string("123stella"));
	BitMRC bitmrc;

	bitmrc.start();

	bitmrc.getPubKey(address);

	while (address.waitingPubKey())
		Sleep(100);

	//bitmrc.Nodes.push_back(new NodeConnection("0000:0000:0000:0000:0000:ffff:5fa5:a8a8","8444", bitmrc));
	/*
	sTag tag1;
	strncpy(tag1.ch, (const char *)address.getTag().c_str(),32);

	// Pretty print key
	encoded.clear();
	StringSource((BYTE*)tag1.ch, 32, true,
		new HexEncoder(
			new StringSink(encoded)
			) // HexEncoder
		); // StringSource
	cout << "tag: " << encoded << endl;
	*/

	//packet_getdata getpacket;
	//getpacket.inventory.clear();
	//while (bitmrc.Hashes.size() == 0)
	//	Sleep(100);
	//sTag tag = bitmrc.Hashes[0];
	//getpacket.inventory.push_back(tag);
	//getpacket.encodeData();

	//getpacket.setChecksum_Lenght_Magic();

	//bitmrc.new_packets.push(getpacket);
	while (1)
	{
		char command[256];
		scanf("%s\n", command);
		if (!strcmp("quit", command))
			break;
		else if (!strcmp("send", command))
		{
			if (bitmrc.PrivAddresses.size() == 0)
			{
				Addr *privateAddr = new Addr;
				privateAddr->generateRandom();
				Addr *privateAddr2 = new Addr;
				privateAddr2->generateRandom();

				bitmrc.new_PrivKey.push(privateAddr);
				bitmrc.new_PrivKey.push(privateAddr2);
			}

			file_ustring data;
			data.setFile(stdin);
			ustring msg = data.getUstring(4);
			bitmrc.sendMessage(msg, address, *bitmrc.PrivAddresses[0]);
		}
		//bitmrc.new_packets.push(getpacket);
	}

	return 0;
}