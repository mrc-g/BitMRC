#include "NodeConnection.h"

/* \todo: provide baseclass, and subclass for every platform instead of 
feature-toggle-it-out -steady286- */
NodeConnection::NodeConnection(BitMRC *self)
{
#ifndef LINUX
	this->wsaInit = WSAStartup(MAKEWORD(2,2), &this->wsaData);
#endif
	CLRMEM(hints);
	this->Socket = INVALID_SOCKET;
	this->result = NULL;
	this->state = 0;

	this->hints.ai_family = AF_UNSPEC;
    this->hints.ai_socktype = SOCK_STREAM;
    this->hints.ai_protocol = IPPROTO_TCP;

	this->bitmrc = self;
}

NodeConnection::NodeConnection(string ip, string port, BitMRC *self)
{
	this->Ip = ip;
	this->Port = port;

	this->state = 0;
#ifndef LINUX
	this->wsaInit = WSAStartup(MAKEWORD(2,2), &this->wsaData);
#endif
	CLRMEM(hints);
	this->Socket = INVALID_SOCKET;

	this->result = NULL;

	this->hints.ai_family = AF_UNSPEC;
    this->hints.ai_socktype = SOCK_STREAM;
    this->hints.ai_protocol = IPPROTO_TCP;

	this->bitmrc = self;
}

NodeConnection::~NodeConnection()
{
	if(this->wsaInit == 0)
	{
#ifndef LINUX
		WSACleanup();
#endif
	}
	if(this->state)
	{
		this->Close();
	}

	this->Packets.push(Packet());

	if(this->thread_listener.joinable())
		this->thread_listener.join();
	if (this->thread_sender.joinable())
		this->thread_sender.join();
}

bool NodeConnection::SetIpPort(string ip, string port)
{
	if(!this->state)
	{
		return false;
	}
	this->Ip = ip;
	this->Port = port;
	return true;
}

bool NodeConnection::SetIpPortIPV6(string ip, string port)
{
	if(!this->state)
	{
		return false;
	}
	this->Ip = ip;
	this->Port = port;
	this->hints.ai_family = AF_INET6;
	//this->hints.ai_flags = AI_NUMERICHOST;
	this->hints.ai_flags = AI_V4MAPPED;

	return true;
}

bool NodeConnection::setTimeout(int t)
{
	if(!state)
		return false;

	int e = setsockopt(Socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&t, sizeof(int));
	if (e == SOCKET_ERROR)
	{
        //wprintf(L"getsockopt for SO_RCVTIMEO failed with error: %u\n", WSAGetLastError());
		return false;
    }
	return true;
}

void NodeConnection::Start()
{
	this->thread_listener = thread(&NodeConnection::Connect,this);
}
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
        return &(((struct sockaddr_in*)sa)->sin_addr);
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
bool NodeConnection::Connect()
{
	if(this->wsaInit != 0)
		return false; //problem with winsock

	struct addrinfo *ptr = NULL;
    int iResult;

    // Resolve the server address and port
	iResult = getaddrinfo(this->Ip.c_str(), this->Port.c_str(), &hints, &result);
    if ( iResult != 0 )
	{
        //printf("getaddrinfo failed with error: %d\n", iResult);
#ifndef LINUX
        WSACleanup();
#endif
        return false;
    }

    // Attempt to connect to an address until one succeeds
    for(ptr=result; ptr != NULL ;ptr=ptr->ai_next)
	{

        // Create a SOCKET for connecting to server
        Socket = socket(ptr->ai_family, ptr->ai_socktype, 
            ptr->ai_protocol);
        if (Socket == INVALID_SOCKET)
		{
            //printf("socket failed with error: %ld\n", WSAGetLastError());
			freeaddrinfo(result);
#ifndef LINUX
            WSACleanup();
#endif
            return false;
        }

        // Connect to server.
		//char buffer[600];
		//const char *string = inet_ntop(ptr->ai_addr->sa_family, get_in_addr((struct sockaddr *)ptr->ai_addr), buffer, sizeof(buffer));
		//printf("%s", buffer);
        iResult = connect( Socket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR)
		{
			closesocket(Socket);
            Socket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (Socket == INVALID_SOCKET)
	{
        //printf("Unable to connect to server!\n");
#ifndef LINUX
        WSACleanup();
#endif
        return false;
    }
	//printf("Connected: %s!\n",this->Ip.c_str());
	this->state = 1;

	this->thread_sender = thread(&NodeConnection::Sender, this);

	packet_version version;
	version.magic = Magic;
	
	version.addr_recv.services = NODE_NETWORK;
	int ip[4];
	sscanf(this->Ip.c_str(),"%i.%i.%i.%i",&ip[0],&ip[1],&ip[2],&ip[3]);
	char arr[16] = {(char)0x00,(char)0x00,(char)0x00,(char)0x00,(char)0x00,(char)0x00,(char)0x00,(char)0x00,
					(char)0x00,(char)0x00,(char)0xff,(char)0xff,(char)ip[0],(char)ip[1],(char)ip[2],(char)ip[3]};
	for(int i=0; i<16;i++)
		version.addr_recv.IPv6_4.appendInt8(arr[i]);
	int port;
	sscanf(this->Port.c_str(),"%i",&port);
	version.addr_recv.port = (unsigned __int32)port;

	version.addr_from.services = NODE_NETWORK;
	char arr2[16] = {	(char)0x00,(char)0x00,(char)0x00,(char)0x00,(char)0x00,(char)0x00,(char)0x00,(char)0x00,
						(char)0x00,(char)0x00,(char)0xff,(char)0xff,(char)0x7f,(char)0x00,(char)0x00,(char)0x01};
	for(int i=0; i<16;i++)
		version.addr_from.IPv6_4.appendInt8(arr2[i]);
	sscanf(this->Port.c_str(),"%i",&port);
	version.addr_from.port = (unsigned __int32)port;

	version.nonce = -5399599167942985965;
	version.stream_numbers.appendInt8(1);

	version.encodeData();

	version.setChecksum_Lenght_Magic();
	try
	{
		version.sendData(this->Socket);
	}
	catch (int e)
	{
		switch (e)
		{
		case CONNECTION_CLOSED:
		case CONNECTION_ERROR:
			this->Close();
			return false;
		}
	}
	this->setTimeout(10*60*1000); //10 min
	this->Listener();
	return true;
}

void NodeConnection::Close()
{
	if(!this->state)
		return;

	this->state = 0;
	// cleanup socket
    closesocket(this->Socket);
}

void NodeConnection::Sender()
{
	try
	{
		while (this->Socket != SOCKET_ERROR && this->state != 0)
		{
			if (this->state == 2)
			{
				Packet packet = Packets.pop();

				packet.sendData(this->Socket);


				std::uniform_int_distribution<int> distribution(0, 2000);
				std::mt19937 engine;
				std::random_device rd;
				engine.seed(rd());
				int random = distribution(engine);

				SLEEP(100 + random); //this delay should prevent some attack
			}
		}
	}
	catch (int e)
	{
		switch (e)
		{
		case CONNECTION_CLOSED:
		case CONNECTION_ERROR:
			this->Close();
			return;
		}
	}
}

void NodeConnection::Listener()
{
	try
	{
		while(this->Socket != SOCKET_ERROR)
		{
			Packet  packet;
			
			packet.getCommand(this->Socket);

			//printf("%s\n", packet.command); //this printf are not really thread save, and they are only needed for debug

			if (!strcmp(packet.command, "pong")) //just a keep alive
			{
				packet.sendData(this->Socket); //resending it
			}else if(!strcmp(packet.command,"version"))
			{
				packet_version version(packet);
				this->Version = version.version;
			}else if(!strcmp(packet.command,"verack"))
			{
				packet_verack verack(packet);

				verack.sendData(this->Socket);

				//empty addr for now
				packet_addr addr;
				addr.addr_list.clear();
				addr.encodeData();

				addr.setChecksum_Lenght_Magic();


				addr.sendData(this->Socket);

				//empty inv for now, this should be the whole list of object hash
				bool finished = false;
				vector<sTag> inventory;
				std::shared_lock<std::shared_timed_mutex> mlock(bitmrc->sharedObj.mutex_);
				for (int i = 0; i < bitmrc->sharedObj.Dim; i++)
				{
					hash_table<ustring>::linked_node * cur = bitmrc->sharedObj.Table[i];
					while (cur != NULL)
					{
						sTag hash;
						memcpy(hash.ch, cur->hash.c_str(), 32);
						inventory.push_back(hash);

						cur = cur->next;
					}
				}
				mlock.unlock();
				unsigned int i = 0;
				while (!finished)
				{
					packet_inv inv;
					while (i < 50000 && i < inventory.size())
					{
						inv.inventory.push_back(inventory[i]);
						i++;
					}
					inv.encodeData();

					inv.setChecksum_Lenght_Magic();


					inv.sendData(this->Socket);
					if (i == inventory.size())
						finished = true;
				}
				this->state = 2;
			}else if(!strcmp(packet.command,"addr"))
			{
				packet_addr addr(packet);
				for(unsigned int i=0;i<addr.addr_list.size();i++)
				{
					if(addr.addr_list[i].IPv6_4[11] == 0xff &&  addr.addr_list[i].IPv6_4[10] == 0xff)
					{
						char buf[40];
						sprintf(buf,"%i.%i.%i.%i", addr.addr_list[i].IPv6_4[12], addr.addr_list[i].IPv6_4[13], addr.addr_list[i].IPv6_4[14], addr.addr_list[i].IPv6_4[15]);
						string ip(buf);

						//printf("%s\n",buf);

						sprintf(buf, "%i", addr.addr_list[i].port);
						string port(buf);

						NodeConnection *tmp_node= new NodeConnection(ip,port, this->bitmrc);

						this->bitmrc->connectNode(tmp_node);
					}	
				}
			}else if(!strcmp(packet.command,"inv"))
			{
				packet_inv inv(packet);
				packet_getdata needed;
				for (unsigned int i = 0; i < inv.inventory.size(); i++)
				{
					ustring tag;
					for (int j = 0; j < 32; j++)
					{
						tag += inv.inventory[i].ch[j];
					}
					
					
					if (this->bitmrc->sharedObj.searchByHash(tag).empty())
					{
						needed.inventory.push_back(inv.inventory[i]);
					}
				}
				if (!needed.inventory.empty())
				{
					needed.encodeData();
					needed.setChecksum_Lenght_Magic();
					Packets.push(needed);
				}
			}else if(!strcmp(packet.command,"getdata"))
			{
				packet_getdata getdata(packet);

				for (unsigned int i = 0; i < getdata.inventory.size(); i++)
				{
					ustring tag;
					for (int j = 0; j < 32; j++)
					{
						tag += getdata.inventory[i].ch[j];
					}
					ustring ObjPayload = this->bitmrc->sharedObj.searchByHash(tag);
					if (!ObjPayload.empty())
					{
						object obj;
						
						memset(obj.command, 0x00, sizeof obj.command);
						strncpy(obj.command, "object", 7);

						obj.message_payload = ObjPayload;
						obj.setChecksum_Lenght_Magic();
						Packets.push(obj);
					}
				}
			}else if(!strcmp(packet.command,"object"))
			{
				object obj(packet);
				bool check = checkPow(obj.message_payload, obj.Time);

				//if not ignore
				if (check)
				{
					ustring invHash = this->bitmrc->inventoryHash(obj.message_payload);
					int present = this->bitmrc->sharedObj.insert(obj.message_payload, invHash);
					
					sTag tag;
					memcpy(tag.ch, invHash.c_str(), 32);

					this->bitmrc->new_inv.push(tag);

					if (obj.objectType == type_getpubkey)
					{
						packet_getpubkey getpubkey(obj);
						
						std::shared_lock<std::shared_timed_mutex> mlock(this->bitmrc->mutex_priv);

						for (unsigned int i = 0; i < this->bitmrc->PrivAddresses.size(); i++)
						{
							ustring tag = this->bitmrc->PrivAddresses[i].getTag();
							if (getpubkey.tag == tag)
							{
								if(this->bitmrc->PrivAddresses[i].getLastPubKeyRequest() + 60 * 60 * 24 * 4 < time(NULL))
									this->bitmrc->sendObj(this->bitmrc->PrivAddresses[i].encodePubKey(),true);
								//else
									//printf("PubKey already shared recently");
							}
						}

						mlock.unlock();
					}
					else if (obj.objectType == type_pubkey)
					{
						packet_pubkey pubkey(obj);

						std::shared_lock<std::shared_timed_mutex> mlock(this->bitmrc->mutex_pub);

						for (unsigned int i = 0; i < this->bitmrc->PubAddresses.size(); i++)
						{
							if (!this->bitmrc->PubAddresses[i].waitingPubKey())
								continue;
							ustring tag = this->bitmrc->PubAddresses[i].getTag();
							if (pubkey.tag == tag)
							{
								this->bitmrc->PubAddresses[i].decodeFromObj(pubkey);
							}
						}

						mlock.unlock();
					}
					else if (obj.objectType == type_msg)
					{
						packet_msg msg(obj);

						if (this->bitmrc->decryptMsg(msg)) //it takes like 1-4 milliseconds
						{
							//printf("Message accepted\n");
						}
					}
					else if (obj.objectType == type_broadcast)
					{
						packet_broadcast broadcast(obj);

						if (this->bitmrc->decryptMsg(broadcast))
						{
							//printf("broadcast decrypted\n");
						}
					}
				}
			}
			
		}
	}catch(int e)
	{
		switch(e)
		{
		case CONNECTION_CLOSED:
		case CONNECTION_ERROR:
			this->Close();
			return;
		}
	}
}
