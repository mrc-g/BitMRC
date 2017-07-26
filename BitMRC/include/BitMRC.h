#pragma once
#ifndef _BITMRC
#define _BITMRC

#include <vector>
#include <random>

#include "Addr.h"
#include <Storage/Storage_mysql.h>
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

	//starts network
	void init();

	~BitMRC();
	vector<NodeConnection*> Nodes;
	vector<PubAddr> PubAddresses; //do not add manually use saveAddr
	vector<Addr> PrivAddresses; //same as above
	vector<PubAddr> Subscriptions; //same

	Queue<Packet> new_packets;

	Queue<message> new_messages; //the new messages not already stored in messages will be there

	Queue<sTag> new_inv; //this will store the new inv to be shared with every node, done by another thread

	//store a queue for the object that needs POW and to be propagated
	Queue<object> objects;

	hash_table<message> messages;

	hash_table<ustring> sharedObj;

	std::shared_timed_mutex mutex_priv;
	std::shared_timed_mutex mutex_pub;
	std::shared_timed_mutex mutex_nodes;
	std::shared_timed_mutex mutex_subs;

	//starts threads
	void start();

	//process an incoming object, this is probably done in the connections sockets
	void processObj(object obj);

	void getPubKey(PubAddr address);


	void sendMessage(ustring message, PubAddr toAddr, Addr fromAddr);
	void sendBroadcast(ustring message, Addr address);

	void sendObj(object payload);
	void processPOW();

	//check and propagate the inv intead of sending the obj directly
	void propagate(object obj);

	//send the packet directly
	void send(Packet packet);

	bool validateObj(object obj);

	bool decryptMsg(packet_msg msg);
	bool decryptMsg(packet_broadcast msg);

	//generate n deterministic address with the passphrase
	void generateDeterministicAddr(ustring passphrase, int n);

	void addAddr(PubAddr address);
	void addAddr(Addr address);
	void addSubscription(PubAddr address);

	Storage_mysql * getStorage();

	void connectNode(NodeConnection *node);

	thread thread_new_packets;
	thread thread_new_inv;
	thread thread_init;
	thread thread_object_pow;

	bool running;

	void listen_packets();
	void listen_inv();

	ustring inventoryHash(ustring data);

	void load(string path);
	void save(string path);

	Storage_mysql * strg = NULL;

};


#endif
