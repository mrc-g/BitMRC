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
	while (bitmrc.PubAddresses.size() == 0)
		Sleep(100);
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
				Addr privateAddr;
				privateAddr.generateRandom();
				Addr privateAddr2;
				privateAddr2.generateRandom();

				bitmrc.new_PrivKey.push(privateAddr);
				bitmrc.new_PrivKey.push(privateAddr2);
			}

			while (bitmrc.PrivAddresses.size() == 0)
				Sleep(100);

			file_ustring data;
			data.setFile(stdin);
			ustring msg = data.getUstring(4);
			bitmrc.sendMessage(msg, bitmrc.PubAddresses[0], bitmrc.PrivAddresses[0]);
			printf("messaggio inviato");
		}
	}

	return 0;
}