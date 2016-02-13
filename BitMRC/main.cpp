#include "main.h"

//just an example

int main()
{

	ustring addre;
	addre.fromString(string("BM-2cV1F1Ae2V5DtvTYhnNw81gvTyttH2SBYx")); //just a test address
	PubAddr address;
	bool loaded = address.loadAddr(addre);

	ustring ciao;
	ciao.fromString(string("12345"));
	BitMRC bitmrc;

	bitmrc.start();

	bitmrc.getPubKey(address);

	while (address.waitingPubKey())
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
				Addr *privateAddr = new Addr;
				privateAddr->generateRandom();
				Addr *privateAddr2 = new Addr;
				privateAddr2->generateRandom();

				bitmrc.new_PrivKey.push(privateAddr);
				bitmrc.new_PrivKey.push(privateAddr2);
			}

			file_ustring data;
			data.setFile(stdin);
			ustring msg = data.getUstring(4);
			bitmrc.sendMessage(msg, address, *bitmrc.PrivAddresses[0]);
		}
	}

	return 0;
}