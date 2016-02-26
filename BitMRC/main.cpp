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

	bitmrc.getPubKey(address);

	//no need to wait address to be added: now is sync

	while (bitmrc.PubAddresses[0].waitingPubKey())
		Sleep(100);

	while (1)
	{
		char command[256];
		scanf("%s\n", command);
		if (!strcmp("quit", command))
			break;
		else if (!strcmp("send", command))
		{
			if (bitmrc.PrivAddresses.size() == 0)
			{
				printf("No private address\n");
				continue;
			}

			ustring msg;
			scanf("%s", command);
			msg.fromString(command);
			bitmrc.sendMessage(msg, bitmrc.PubAddresses[0], bitmrc.PrivAddresses[0]);
			printf("messaggio inviato");
		}
		else if (!strcmp("generate", command))
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
			}
		}
	}

	return 0;
}