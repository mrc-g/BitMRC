#pragma once
#ifndef _BITMRC
#define _BITMRC

#include <vector>

#include "Addr.h"

class BitMRC;

#include "NodeConnection.h"

class BitMRC
{
public:

	struct message
	{
		ustring from;
		ustring to;
		ustring info;
	};

	BitMRC();
	~BitMRC();
	vector<NodeConnection*> Nodes;
	vector<PubAddr> PubAddresses; //do not add manually use saveAddr
	vector<Addr> PrivAddresses; //same as above

	Queue<NodeConnection*> new_ip; //do not delete manually

	Queue<Packet> new_packets;

	Queue<ustring> messages;
	
	hash_table<ustring> sharedObj;

	std::mutex mutex_priv;
	std::mutex mutex_pub;
	//std::mutex mutex_hash;

	void start();

	void getPubKey(PubAddr address);

	void sendMessage(ustring message, PubAddr toAddr, Addr fromAddr);

	void sendObj(object payload);

	bool decryptMsg(packet_msg msg);

	//generate n deterministic address with the passphrase
	void generateDeterministicAddr(ustring passphrase, int n);

	void saveAddr(PubAddr address);
	void saveAddr(Addr address);

	void connectNode();

	thread thread_new_ip;
	thread thread_new_hashes;
	thread thread_new_packets;

	bool running;

	void listen_ips();
	void listen_hashes();
	void listen_packets();

	ustring inventoryHash(ustring data);

	void load(string path);
	void save(string path);

};


#endif