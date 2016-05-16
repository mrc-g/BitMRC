/*
 * NodeBlacklist.h
 *
 *  Created on: 06.05.2016
 *      Author: steady286
 */

#ifndef NODEBLACKLIST_H_
#define NODEBLACKLIST_H_

using namespace std;

#include <cstdio>
#include <cstring>
#ifdef LINUX
#include <arpa/inet.h>

#else
#include <WinSock2.h>
#endif

class NodeBlacklist {
public:
	NodeBlacklist();
	~NodeBlacklist();
	int is_blacklisted(string ip, uint32_t ip_v);
	int is_blacklisted(struct addrinfo * address_in, int family);
	// todo: implement
	int add_blacklist_entry(std::string ip, uint32_t ip_v);
	// testing the detection on test vectors
	int NodeBlacklist::test();
	int NodeBlacklist::test_binaddr();
};

#endif /* NODEBLACKLIST_H_ */
