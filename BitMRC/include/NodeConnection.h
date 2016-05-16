#pragma once
#ifndef _NODE_CONNECTION
#define _NODE_CONNECTION

#include <cstdio>
#include <cstring>
#include <vector>
#include <thread>
#include <future>
#include <iostream>
#include <random>
#ifdef LINUX
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#else
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#endif


#include "utils.h"
#include "Packet.h"
#include "pow.h"



#ifndef LINUX
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
#endif

using namespace std;

class NodeConnection;

#include "BitMRC.h"

class NodeConnection
{
public:
	NodeConnection(BitMRC *asd);
	NodeConnection(string ip, string port, BitMRC *asd);
	~NodeConnection();

	void Start();

	//connect to the current ip/port
	bool Connect();
	void Close();

	//change or set ip and port only if is not connected
	bool SetIpPort(string ip, string port);

	//change or set ip and port only if is not connected
	bool SetIpPortIPV6(string ip, string port);

	//set timeout in millisec
	bool setTimeout(int t);

	thread thread_listener;
	thread thread_sender;

	BitMRC *bitmrc;

	//holds ip
	string Ip;
	//holds port
	string Port;

	//state of connection:
	//		0: not connected
	//		1: connected
	//		2: connection accepted
	int state;

	//version
	int Version;

	//holds addrss structure
	struct addrinfo *result,hints;

	//holds WSADATA
	WSADATA wsaData;
	//holds state of wsadata
	int wsaInit;
	//holds the socket
	SOCK_TYPE Socket;
	
	//packets queue for multi threading
	Queue<Packet> Packets;

	//the listener of the protocol
	void Listener();
	//the sender
	void Sender();
};

#endif
