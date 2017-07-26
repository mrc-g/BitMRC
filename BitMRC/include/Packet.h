#ifndef _PACKET
#define _PACKET

#include <ctime>
#include <cstdio>
#ifdef LINUX

#else
#include <shared_mutex>
#endif
#include "utils.h"

// Crypto++ Library
//#ifdef _DEBUG
//#  pragma comment ( lib, "cryptlibd.lib" )
//#else
//#  pragma comment ( lib, "cryptlib.lib" )
//#endif
#include <cryptlib.h>
#include "modes.h" // xxx_Mode< >
#include "filters.h" // StringSource and
#include <sha.h>
#include <eccrypto.h>
#include <ripemd.h>
#include <osrng.h>
#include <asn.h>
#include <oids.h>
#include <Storage/Storable.h>
#include <Storage/Unique_Key.h>


using namespace CryptoPP::ASN1;
using namespace CryptoPP;

using namespace std;

const int32_t Version = 3;
#if 0
// PyBitMessage 0.4.4
const char User_Agent[]= {0x2f, 0x50, 0x79, 0x42, 0x69, 0x74, 0x6d,
	0x65, 0x73, 0x73, 0x61, 0x67, 0x65, 0x3a, 0x30,
	0x2e, 0x34, 0x2e, 0x34, 0x2f};
#endif
// BitMRCVersion0.0.1
const char User_Agent[]={0x2F, 0x42, 0x69, 0x74, 0x4D, 0x52, 0x43, 0x56, 0x65, 0x72,
						0x73, 0x69, 0x6F, 0x6E, 0x30, 0x2E, 0x30, 0x2E, 0x31, 0x2F};

enum EncodingType
{
	Ignore = 0,
	Trivial = 1,
	Simple = 2
};

enum ServicesType
{
	NODE_NETWORK = 1,
	NODE_MOBILE = 2,
	NODE_GATEWAY = 4
};

enum ObjectType
{
	type_getpubkey = 0,
	type_pubkey = 1,
	type_msg = 2,
	type_broadcast = 3
};

struct Network_address
{
	uint32_t time;
	uint32_t stream;
	uint64_t services;
	ustring	IPv6_4;
	uint16_t port;
};

struct sTag
{
	char ch[32];
};

const uint32_t Magic = {0xE9BEB4D9};

class Packet : public Storable
{
public:
	Packet(){}

	Packet(ustring data);

	//magic
	uint32_t magic;

	//command null padded
	char command[12];

	//lenght of payload in bytes
	uint32_t length;

	//checksum: first 4 bytes of sha512(payload)
	char checksum[4];

	//payload: can be a message or an object
	ustring message_payload;

	//gets the data until it reaches the command
	void getCommand(SOCK_TYPE socket);

	//sends data
	void sendData(SOCK_TYPE socket);

	bool setChecksum_Lenght_Magic();
public:
	bool set_key(Unique_Key & key_in); // set the unique key manually
	Unique_Key get_key();
	bool query(Unique_Key &uq_key_in, std::string & data_out);
	bool store();
	bool delete_storable(Storable & object_in);
	bool delete_storable(Unique_Key & key_in);
	bool find_by_key(Unique_Key &);
	bool set_storage(Storage*);
	char * get_update_query();
	char * get_load_query();
};

class packet_version : public Packet
{
public:
	packet_version(){}

	packet_version(Packet packet)
	{
		this->magic = packet.magic;
		strncpy(this->command, packet.command,12);
		this->length = packet.length;
		strncpy(this->checksum, packet.checksum,4);
		this->message_payload = packet.message_payload;
		this->decodeData();
	}

	//version
	int32_t version;
	//service
	uint64_t services;
	//UNIX timestamp in seconds
	uint64_t timestamp;
	//the net_addr of the receiver (not including the time and stream number)
	Network_address		addr_recv;
	//the net_addr of the emitting node (not including the time and the stream number and the ip is ignored by the receiver)
	Network_address		addr_from;
	//random nonce used to detect connections to self
	uint64_t nonce;
	//user agent: 0x00 if is 0 bytes long
	ustring	user_agent;
	//list of stream number that the emitting node is interested in, not more than 160000
	ustring	stream_numbers;

	//gets data
	void decodeData() {
		unsigned int i=0;
		this->version = this->message_payload.getInt32(i);
		this->services = this->message_payload.getInt64(i);
		this->timestamp = this->message_payload.getInt64(i);

		this->addr_recv.services = this->message_payload.getInt64(i);
		this->addr_recv.IPv6_4.clear();
		for(int j=0;j<16;j++)
			this->addr_recv.IPv6_4 += (unsigned char)this->message_payload.getInt8(i); //error
		this->addr_recv.port = this->message_payload.getInt16(i);

		this->addr_from.services = this->message_payload.getInt64(i);
		this->addr_from.IPv6_4.clear();
		for(int j=0;j<16;j++)
			this->addr_from.IPv6_4 += (unsigned char)this->message_payload.getInt8(i); //error
		this->addr_from.port = this->message_payload.getInt16(i);

		this->nonce = this->message_payload.getInt64(i);
		this->user_agent = this->message_payload.getVarUstring_B(i);

		this->stream_numbers.clear();
		this->stream_numbers += this->message_payload.substr(i,this->message_payload.length());
	}

	void encodeData()
	{
		strcpy(this->command, "version");

		this->message_payload.clear();
		this->message_payload.appendInt32(Version);
		this->message_payload.appendInt64(NODE_NETWORK);
		this->message_payload.appendInt64(std::time(nullptr));

		this->message_payload.appendInt64(this->addr_recv.services);
		this->message_payload += this->addr_recv.IPv6_4;
		this->message_payload.appendInt16(this->addr_recv.port);

		this->message_payload.appendInt64(this->addr_from.services);
		this->message_payload += this->addr_from.IPv6_4;
		this->message_payload.appendInt16(this->addr_from.port);

		this->message_payload.appendInt64(this->nonce);

		this->user_agent.append((unsigned char*)User_Agent, 20);

		if(this->user_agent.length()==0)
			this->message_payload.append(0);
		else
			this->message_payload.appendVarUstring_B(this->user_agent);

		this->message_payload.appendVarUstring_B(this->stream_numbers);
	}
};

class packet_verack : public Packet
{
public:
	packet_verack(){}

	packet_verack(Packet packet)
	{
		this->magic = packet.magic;
		strncpy(this->command, packet.command,12);
		this->length = packet.length;
		strncpy(this->checksum, packet.checksum,4);
		this->message_payload = packet.message_payload;
	}
};

class packet_addr : public Packet
{
public:
	packet_addr(){}

	packet_addr(Packet packet)
	{
		this->magic = packet.magic;
		strncpy(this->command, packet.command,12);
		this->length = packet.length;
		strncpy(this->checksum, packet.checksum,4);
		this->message_payload = packet.message_payload;
		this->decodeData();
	}

	//number of address entries (max: 1000)
	//address of other nodes on the network
	vector<Network_address>	addr_list;

	//decodes data
	void decodeData()
	{
		unsigned int i=0;
		int64_t size=0;
		size = this->message_payload.getVarInt_B(i);
		for(int j=0;j<size;j++)
		{
			Network_address tmp;
			tmp.time = this->message_payload.getInt32(i);
			tmp.stream = this->message_payload.getInt32(i);
			tmp.services = this->message_payload.getInt64(i);
			tmp.IPv6_4 += this->message_payload.getUstring(16,i);
			tmp.port = this->message_payload.getInt16(i);
			addr_list.push_back(tmp);
		}
	}

	void encodeData()
	{
		strcpy(this->command, "addr");
		this->message_payload.clear();
		this->message_payload.appendVarInt_B(this->addr_list.size());
		for(unsigned int i=0;i<this->addr_list.size();i++)
		{
			this->message_payload.appendInt32(this->addr_list[i].time);
			this->message_payload.appendInt32(this->addr_list[i].stream);
			this->message_payload.appendInt64(this->addr_list[i].services);
			this->message_payload += this->addr_list[i].IPv6_4;
			this->message_payload.appendInt16(this->addr_list[i].port);
		}
	}

};

class packet_inv : public Packet
{
public:
	packet_inv(){}

	packet_inv(Packet packet)
	{
		this->magic = packet.magic;
		strncpy(this->command, packet.command,12);
		this->length = packet.length;
		strncpy(this->checksum, packet.checksum,4);
		this->message_payload = packet.message_payload;
		this->decodeData();
	}

	//number of inventory entries
	//inventory vectors (max: 50000)
	vector<sTag>	inventory;

	//decodes data
	void decodeData()
	{
		unsigned int i=0;
		int64_t size = this->message_payload.getVarInt_B(i);
		for (int j = 0; j < size; j++)
		{
			sTag tag;
			for (int k = 0; k < 32; k++)
			{
				tag.ch[k] = this->message_payload.getInt8(i);
			}
			inventory.push_back(tag);
		}
	}

	void encodeData()
	{
		strcpy(this->command, "inv");
		this->message_payload.clear();
		this->message_payload.appendVarInt_B(this->inventory.size());
		for(unsigned int i=0;i<this->inventory.size();i++)
		{
			this->message_payload.append((unsigned char*)this->inventory[i].ch, 32);
		}
	}
};

class packet_getdata : public Packet
{
public:
	packet_getdata(){}

	packet_getdata(Packet packet)
	{
		this->magic = packet.magic;
		strncpy(this->command, packet.command,12);
		this->length = packet.length;
		strncpy(this->checksum, packet.checksum,4);
		this->message_payload = packet.message_payload;
		this->decodeData();
	}

	//number of inventory entries
	//Inventory vectors (max: 50000)
	vector<sTag>	inventory;

	//decodes data
	void decodeData()
	{
		unsigned int i = 0;
		int64_t size = this->message_payload.getVarInt_B(i);
		for (int j = 0; j < size; j++)
		{
			sTag tag;
			for (int k = 0; k < 32; k++)
			{
				tag.ch[k] = this->message_payload.getInt8(i);
			}
			inventory.push_back(tag);
		}
	}

	void encodeData()
	{
		strcpy(this->command, "getdata");
		this->message_payload.clear();
		this->message_payload.appendVarInt_B(this->inventory.size());
		for (unsigned int i = 0; i<this->inventory.size(); i++)
		{
			this->message_payload.append((unsigned char*)this->inventory[i].ch, 32);
		}
	}
};

class object : public Packet
{
public:

	object(){}


	object(Packet packet)
	{
		this->magic = packet.magic;
		strncpy(this->command, packet.command,12);
		this->length = packet.length;
		strncpy(this->checksum, packet.checksum,4);
		this->message_payload = packet.message_payload;
		this->decodeData();
	}

	//random nonce used for the Proof Of Work
	uint64_t nonce;

	//TTL (should be in seconds): not more than 28 days + 3 hours in the future
	int64_t	Time;

	//object type to be decoded.
	uint32_t objectType;

	//object payload, the fields depend on the object type
	ustring	objectPayload;

	//object version
	uint64_t version;

	//stream number
	uint64_t stream;

	//decodes data
	void decodeData()
	{
		unsigned int i = 0;
		this->nonce = this->message_payload.getInt64(i);
		this->Time = this->message_payload.getInt64(i);
		this->objectType = this->message_payload.getInt32(i);
		this->version = this->message_payload.getVarInt_B(i);
		this->stream = this->message_payload.getVarInt_B(i);
		this->objectPayload = this->message_payload.getRest(i);
	}

	void encodePayload()
	{
		this->message_payload.clear();
		this->message_payload.appendInt64(this->nonce);
		this->message_payload.appendInt64(this->Time);
		this->message_payload.appendInt32(this->objectType);
		this->message_payload.appendVarInt_B(this->version);
		this->message_payload.appendVarInt_B(this->stream);
		this->message_payload += this->objectPayload;
	}
};

class packet_getpubkey : public object
{
public:
	packet_getpubkey(){}

	packet_getpubkey(object obj)
	{
		this->magic = obj.magic;
		strncpy(this->command, obj.command,12);
		this->length = obj.length;
		strncpy(this->checksum, obj.checksum,4);
		this->message_payload = obj.message_payload;
		this->nonce = obj.nonce;
		this->Time = obj.Time;
		this->objectType = obj.objectType;
		this->version = obj.version;
		this->stream = obj.stream;
		this->objectPayload = obj.objectPayload;
		this->decodeObject();
	}

	bool isTag;

	//tag if version is >=4
	ustring tag;

	//decodes data
	void decodeObject()
	{
		if (this->objectPayload.length() == 32)
		{
			isTag = true;
		}
		else if(this->objectPayload.length() == 20)
		{
			isTag = false;
		}
		tag = this->objectPayload;
	}

	void encodeObject()
	{
		this->objectPayload = tag;
	}
};

class packet_pubkey : public object
{
public:
	packet_pubkey();
	packet_pubkey(object obj);
	//version 4
	//tag 32 byte
	ustring tag;
	//encrypted data
	ustring encrypted;
	//decodes data
	void decodeObject();
	void encodeObject();
	// storable interface
	bool set_key(Unique_Key & key_in);
	Unique_Key get_key();
	bool query(Unique_Key &uq_key_in, std::string & data_out);
	bool store();
	bool delete_storable(Storable & object_in);
	bool delete_storable(Unique_Key & key_in);
	bool find_by_key(Unique_Key &);
	bool set_storage(Storage*);
	char * get_update_query();
	char * get_load_query();
};

class packet_msg : public object
{
public:
	packet_msg(){}

	packet_msg(object obj)
	{
		this->magic = obj.magic;
		strncpy(this->command, obj.command,12);
		this->length = obj.length;
		strncpy(this->checksum, obj.checksum,4);
		this->message_payload = obj.message_payload;
		this->nonce = obj.nonce;
		this->Time = obj.Time;
		this->objectType = obj.objectType;
		this->version = obj.version;
		this->stream = obj.stream;
		this->objectPayload = obj.objectPayload;
		this->decodeObject();
	}

	//decodes data
	void decodeObject(){}

	void encodeObject();
};

class packet_broadcast : public object
{
public:
	packet_broadcast() {}

	packet_broadcast(object obj)
	{
		this->magic = obj.magic;
		strncpy(this->command, obj.command, 12);
		this->length = obj.length;
		strncpy(this->checksum, obj.checksum, 4);
		this->message_payload = obj.message_payload;
		this->nonce = obj.nonce;
		this->Time = obj.Time;
		this->objectType = obj.objectType;
		this->version = obj.version;
		this->stream = obj.stream;
		this->objectPayload = obj.objectPayload;
		this->decodeObject();
	}

	//tag 32 bytes
	ustring tag;

	ustring encrypted;


	//decodes data
	void decodeObject()
	{
		if (this->version == 4)
			this->encrypted = this->objectPayload;

		if (this->version == 5)
		{
			unsigned int i = 0;
			this->tag = this->objectPayload.getUstring(32, i);
			this->encrypted = this->objectPayload.getRest(i);
		}

	}

	void encodeObject()
	{
		if (this->version == 4)
			this->objectPayload = this->encrypted;
		if (this->version == 5)
		{
			this->objectPayload = this->tag;
			this->objectPayload += this->encrypted;
		}
	}
};
#endif
