#include "main.h"

//just an example

int main()
{

	ustring addre;
	addre.fromString(string("BM-2cUwUoj9YKoxsZMKT7C5N46tC1KTdUQZpH")); //just a test address
	PubAddr address;
	bool loaded = address.loadAddr(addre);

	ustring ciao;
	ciao.fromString(string("12345"));
	BitMRC bitmrc;

	bitmrc.start();

	if(loaded)
		bitmrc.getPubKey(address);

	//no need to wait address to be added: now is sync

	while (bitmrc.PubAddresses[0].waitingPubKey())
		Sleep(100);

	while (1)
	{
		char command[256];
		scanf("%s", command);
		if (!strcmp("quit", command))
			break;
		else if (!strcmp("send", command))
		{
			if (bitmrc.PrivAddresses.size() == 0)
			{
				printf("No private address\n");
				continue;
			}

			if (bitmrc.PubAddresses.size() == 0)
			{
				printf("No public address\n");
				continue;
			}
			for (int i = 0; i < bitmrc.PubAddresses.size(); i++)
			{
				printf("[%d] Public key: %s Ready: %s\n", i, bitmrc.PubAddresses[i].getAddress().c_str(), bitmrc.PubAddresses[i].waitingPubKey() ? "False" : "True");
			}
			printf("Select destination: ");
			int des;
			scanf("%d", &des);
			if (des < 0 || des >= bitmrc.PubAddresses.size())
			{
				printf("Incorrect selection\n");
				continue;
			}
			for (int i = 0; i < bitmrc.PrivAddresses.size(); i++)
			{
				printf("[%d] Private key: %s\n", i, bitmrc.PrivAddresses[i].getAddress().c_str());
			}
			printf("Select destination: ");
			int fro;
			scanf("%d", &fro);
			if (fro < 0 || fro >= bitmrc.PubAddresses.size())
			{
				printf("Incorrect selection\n");
				continue;
			}

			ustring msg;
			printf("Message to send:\n");
			scanf("%s", command);
			msg.fromString(command);
			bitmrc.sendMessage(msg, bitmrc.PubAddresses[des], bitmrc.PrivAddresses[fro]);
			printf("messaggio inviato\n");
		}
		else if (!strcmp("address", command))
		{
			scanf("%s", command);
			if (!strcmp("generate", command))
			{
				scanf("%s", command);
				if (!strcmp("deterministic", command))
				{
					scanf("%s", command);
					ustring pass;
					pass.fromString(command);
					bitmrc.generateDeterministicAddr(pass, 1);
				}
				else if (!strcmp("random", command))
				{
					Addr privateAddr;
					privateAddr.generateRandom();

					bitmrc.saveAddr(privateAddr);
					printf("Generated %s\n", privateAddr.getAddress().c_str());
				}
			}
			else if (!strcmp("public", command))
			{
				scanf("%s", command);
				ustring addre;
				addre.fromString(string(command));
				PubAddr address;
				bool loaded = address.loadAddr(addre);
				if (loaded)
				{
					printf("Asking public key!\n");
					bitmrc.getPubKey(address);
					printf("Asked! check publickey for news on that address!\n");
				}
				else
					printf("Address not correct!\n");
			}
		}
		else if (!strcmp("check", command))
		{
			scanf("%s", command);
			if (!strcmp("message", command))
			{
				//this should be done asynchronous because this will stuck this thread if no message is incoming
				BitMRC::message mess = bitmrc.new_messages.pop();
				printf("New message:\nFrom: %s\nTo: %s\nMessage:\n%s\n", mess.from.c_str(), mess.to.c_str(), mess.info.c_str());
			}
			else if (!strcmp("connections", command))
			{
				std::shared_lock<std::shared_timed_mutex> mlock(bitmrc.mutex_nodes);

				for (int i = 0; i < bitmrc.Nodes.size(); i++)
				{
					if (bitmrc.Nodes[i]->state == 2)
					{
						printf("Node: %s:%s Connected\n", bitmrc.Nodes[i]->Ip.c_str(), bitmrc.Nodes[i]->Port.c_str());
					}
					else if (bitmrc.Nodes[i]->state == 0)
					{
						printf("Node: %s:%s Not connected\n", bitmrc.Nodes[i]->Ip.c_str(), bitmrc.Nodes[i]->Port.c_str());
					}
				}

				mlock.unlock();
			}
			else if (!strcmp("privatekey", command))
			{
				std::shared_lock<std::shared_timed_mutex> mlock(bitmrc.mutex_priv);

				if (bitmrc.PrivAddresses.size() == 0)
					printf("No private key\n");

				for (int i = 0; i < bitmrc.PrivAddresses.size(); i++)
				{
					printf("Private key: %s\n", bitmrc.PrivAddresses[i].getAddress().c_str());
				}

				mlock.unlock();
			}
			else if (!strcmp("publickey", command))
			{
				std::shared_lock<std::shared_timed_mutex> mlock(bitmrc.mutex_pub);
				

				if (bitmrc.PubAddresses.size() == 0)
					printf("No public key\n");

				for (int i = 0; i < bitmrc.PubAddresses.size(); i++)
				{
					printf("Public key: %s Ready: %s\n", bitmrc.PubAddresses[i].getAddress().c_str(), bitmrc.PubAddresses[i].waitingPubKey() ? "False" : "True");
				}

				mlock.unlock();
			}
		}
	}
	return 0;
}