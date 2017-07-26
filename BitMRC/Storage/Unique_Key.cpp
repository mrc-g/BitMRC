/*
 * Unique_Key.cpp
 *
 *  Created on: 01.06.2016
 *      Author: steady286
 */
#include <Addr.h>
#include <Storage/Unique_Key.h>

#include <string>
#include <sha3.h>
#include <ios>
#ifdef DEBUG_UQK
#define DBG_UQK(a) printf a;
#else
#define DBG_UQK(a)
#endif
Unique_Key::Unique_Key()
{
	key.clear();
	digest[0] = '\0';
	digest_len = digest_len_to_use;
}

Unique_Key::~Unique_Key() {

}
bool Unique_Key::create(Addr & in) {
	in.calc_key();
	return false;
}
bool Unique_Key::operator==(Addr & addr_in) {
	return false;
}

bool Unique_Key::set_key(std::string key_in) {
	this->key.assign(key_in);
	return true;
}
uint16_t Unique_Key::get_digest_len() {
	return this->digest_len;
}
std::string Unique_Key::get_key() {
	return this->key;
}

std::string Unique_Key::digest_to_string() {
	ustring converter ;
	return converter.toHexString(this->digest, this->get_digest_len());
}
void Unique_Key::set_digest(const char in[digest_len_to_use]) {
	memcpy(this->digest, in, digest_len_to_use);
}


