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
	BitMRC();
	~BitMRC();
	vector<NodeConnection*> Nodes;
	vector<PubAddr*> PubAddresses;
	vector<Addr*> PrivAddresses;
	vector<sTag> Hashes;

	Queue<NodeConnection*> new_ip; //do not delete manually
	Queue<PubAddr*> new_pubKey; //do not delete manually the key added here
	Queue<Addr*> new_PrivKey; //same as above
	Queue<sTag> new_hashes;

	Queue<Packet> new_packets;

	Queue<ustring> messages;
	
	hash_table sharedObj;

	std::mutex mutex_priv;
	std::mutex mutex_pub;
	std::mutex mutex_hash;

	void start();

	void getPubKey(PubAddr &address);

	void sendMessage(ustring message, PubAddr toAddr, Addr fromAddr);

	void sendObj(object payload);

	bool decryptMsg(packet_msg msg);

	void generateKey();

	void connectNode();

	thread thread_new_ip;
	thread thread_new_PubKey;
	thread thread_new_PrivKey;
	thread thread_new_hashes;
	thread thread_new_packets;

	bool running;

	void listen_ips();
	void listen_pubkeys();
	void listen_privkeys();
	void listen_hashes();
	void listen_packets();

	ustring inventoryHash(ustring data);

	void load(string path);
	void save(string path);

};


#endif