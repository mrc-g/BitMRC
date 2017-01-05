#include "Packet.h"
#include <types.h>
Packet::Packet(ustring data)
{
	unsigned int i = 0;
	this->magic = data.getInt32(i);
	strncpy(this->command, data.getString(12, i).c_str(),12);
	this->length = data.getInt32(i);
	strncpy(this->checksum, data.getString(4,i).c_str(),4);
	this->message_payload = data.getRest(i);
}
void Packet::getCommand(SOCK_TYPE socket)
{
	socket_ustring connection;
	connection.setSocket(socket);
	this->magic = connection.getInt32();
	strncpy(this->command, (const char*)connection.getUstring(12).c_str(), 12);

	this->length = connection.getInt32();
	for (int i = 0; i<4; i++)
		this->checksum[i] = connection.getInt8();
	this->message_payload = connection.getUstring(this->length);
}

void Packet::sendData(SOCK_TYPE socket)
{
	socket_ustring connection;
	connection.setSocket(socket);
	connection.sendInt32(this->magic);
	ustring tmp;
	tmp.append((unsigned char*)this->command, strlen(this->command));
	for (int i = strlen(this->command); i < 12; i++)
		tmp.appendInt8(0);
	connection.sendUstring(tmp, 12);
	connection.sendInt32(this->length);
	for (int i = 0; i<4; i++)
		connection.sendByte((unsigned char)this->checksum[i]);
	connection.sendUstring(this->message_payload, this->length);
}

bool Packet::setChecksum_Lenght_Magic()
{
	this->length = this->message_payload.length();

	BYTE *pbBuffer = new BYTE[this->length + 1];
	for (unsigned int i = 0; i<this->length; i++)
		pbBuffer[i] = this->message_payload[i];
	pbBuffer[this->length] = 0x00;
	DWORD dwBufferLen = this->message_payload.length();
	BYTE rgbHash[512];

	SHA512().CalculateDigest(rgbHash, pbBuffer, dwBufferLen);

	for (int i = 0; i<4; i++)
	{
		this->checksum[i] = rgbHash[i];
	}
	delete[] pbBuffer;
	this->magic = Magic;
	return true;
}
// storable interface
bool Packet::set_key(Unique_Key & key_in) {
	return false;
}
Unique_Key Packet::get_key() {
	Unique_Key k;
	return k;
}
bool Packet::query(Unique_Key &uq_key_in, std::string & data_out) {
	return false;
}
bool Packet::store() {
	return false;
}
bool Packet::delete_storable(Storable & object_in) {
	return false;
}
bool Packet::delete_storable(Unique_Key & key_in) {
	return false;
}
bool Packet::find_by_key(Unique_Key &) {
	return false;
}
bool Packet::set_storage(Storage*) {
	return false;
}
char * Packet::get_update_query() {
	return NULL;
}
char * Packet::get_load_query() {
	return NULL;
}

/** packet_pubkey */
packet_pubkey::packet_pubkey(){

}
packet_pubkey::packet_pubkey(object obj) {
	this->magic = obj.magic;
	strncpy(this->command, obj.command,12);
	this->length = obj.length;
	strncpy(this->checksum, obj.checksum,4);
	this->message_payload = obj.message_payload;
	this->nonce = obj.nonce;
	this->Time = obj.Time;
	this->objectType = obj.objectType;
	this->version = obj.version;
	this->stream = obj.stream;
	this->objectPayload = obj.objectPayload;
	this->decodeObject();
}
void packet_pubkey::encodeObject() {
	if (this->version == 4) {
		this->objectPayload.clear();
		this->objectPayload += this->tag;
		this->objectPayload += this->encrypted;
	}
}
void packet_pubkey::decodeObject() {
	if (this->version == 4) {
		unsigned int i = 0;
		this->tag = this->objectPayload.getUstring(32, i);
		this->encrypted = this->objectPayload.getRest(i);
	}
}
// storable interface
bool packet_pubkey::set_key(Unique_Key & key_in) {
	return false;
}
Unique_Key packet_pubkey::get_key() {
	Unique_Key k;
	return k;
}
bool packet_pubkey::query(Unique_Key &uq_key_in, std::string & data_out) {
	return false;
}
bool packet_pubkey::store() {
	return false;
}
bool packet_pubkey::delete_storable(Storable & object_in) {
	return false;
}
bool packet_pubkey::delete_storable(Unique_Key & key_in) {
	return false;
}
bool packet_pubkey::find_by_key(Unique_Key &) {
	return false;
}
bool packet_pubkey::set_storage(Storage*) {
	return false;
}
char * packet_pubkey::get_update_query() {
	return NULL;
}
char * packet_pubkey::get_load_query() {
	return NULL;
}

