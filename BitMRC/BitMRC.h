#pragma once
#ifndef _BITMRC
#define _BITMRC

#include <vector>
#include <random>

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

		ustring signature;

		bool operator==(const message &that)
		{
			if (((that.from == this->from) && (that.to == this->to)) && (that.info == this->info) && (that.signature == this->signature))
				return true;
			return false;
		}
	};

	BitMRC();
	~BitMRC();
	vector<NodeConnection*> Nodes;
	vector<PubAddr> PubAddresses; //do not add manually use saveAddr
	vector<Addr> PrivAddresses; //same as above

	Queue<Packet> new_packets;

	Queue<message> new_messages; //the new messages not already stored in messages will be there

	Queue<sTag> new_inv; //this will store the new inv to be shared with every node, done by another thread

	hash_table<message> messages;
	
	hash_table<ustring> sharedObj;

	std::shared_timed_mutex mutex_priv;
	std::shared_timed_mutex mutex_pub;
	std::shared_timed_mutex mutex_nodes;

	std::mt19937 engine; //random number generator

	void start();

	void getPubKey(PubAddr address);

	void sendMessage(ustring message, PubAddr toAddr, Addr fromAddr);

	//if only_inv is true this will only create the object, store it and propagate the inv
	//else it will create the object, store it and send it
	void sendObj(object payload, bool only_inv);

	void send(Packet packet);

	bool decryptMsg(packet_msg msg);

	//generate n deterministic address with the passphrase
	void generateDeterministicAddr(ustring passphrase, int n);

	void saveAddr(PubAddr address);
	void saveAddr(Addr address);

	void connectNode(NodeConnection *node);

	thread thread_new_packets;
	thread thread_new_inv;

	bool running;

	void listen_packets();
	void listen_inv();

	ustring inventoryHash(ustring data);

	void load(string path);
	void save(string path);

};


#endif