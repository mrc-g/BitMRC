/*
 * NodeBlacklist.h
 *
 *  Created on: 06.05.2016
 *      Author: blueboxes
 */

#ifndef NODEBLACKLIST_H_
#define NODEBLACKLIST_H_

using namespace std;

#include <cstdio>
#include <cstring>

class NodeBlacklist {
public:
	NodeBlacklist();
	~NodeBlacklist();
	int is_blacklisted(string ip, uint32_t ip_v);
	// todo: implement
	int add_blacklist_entry(string ip, uint32_t ip_v);
	// testing the detection on test vectors
	int NodeBlacklist::test();
};

#endif /* NODEBLACKLIST_H_ */
