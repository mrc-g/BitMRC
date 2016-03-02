#include "Packet.h"

void Packet::getCommand(SOCKET socket)
{
	socket_ustring connection;
	connection.setSocket(socket);
	this->magic = connection.getInt32();
	strncpy(this->command, (const char*)connection.getUstring(12).c_str(), 12);

	this->lenght = connection.getInt32();
	for (int i = 0; i<4; i++)
		this->checksum[i] = connection.getInt8();
	this->message_payload = connection.getUstring(this->lenght);
}

void Packet::sendData(SOCKET socket)
{
	socket_ustring connection;
	connection.setSocket(socket);
	connection.sendInt32(this->magic);
	ustring tmp;
	tmp.append((unsigned char*)this->command, strlen(this->command));
	for (int i = strlen(this->command); i < 12; i++)
		tmp.appendInt8(0);
	connection.sendUstring(tmp, 12);
	connection.sendInt32(this->lenght);
	for (int i = 0; i<4; i++)
		connection.sendByte((unsigned char)this->checksum[i]);
	connection.sendUstring(this->message_payload, this->lenght);
}

bool Packet::setChecksum_Lenght_Magic()
{
	this->lenght = this->message_payload.length();

	BYTE *pbBuffer = new BYTE[this->lenght + 1];
	for (unsigned int i = 0; i<this->lenght; i++)
		pbBuffer[i] = this->message_payload[i];
	pbBuffer[this->lenght] = 0x00;
	DWORD dwBufferLen = this->message_payload.length();
	BYTE rgbHash[512];

	SHA512().CalculateDigest(rgbHash, pbBuffer, dwBufferLen);

	for (int i = 0; i<4; i++)
	{
		this->checksum[i] = rgbHash[i];
	}
	delete pbBuffer;

	this->magic = Magic;

	return true;
}