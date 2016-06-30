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
#define STORABLE_UQK(a) printf a;
#else
#define STORABLE_UQK(a)
#endif
Unique_Key::Unique_Key() {

}
Unique_Key::~Unique_Key() {

}
bool Unique_Key::create_from_addr(Addr & addr_in) {

	return false;
}
bool Unique_Key::operator==(Addr & addr_in) {

	return false;
}

bool Unique_Key::set_key(std::string key_in) {
	unsigned char digest[24];
	SHA3 sha(16);
	sha.CalculateDigest(digest, key_in.c_str(), key_in.length());
	this->key.assign(digest);
	cout << ios::hex << digest ;
//	STORABLE_UQK(("%s: KEY is %s\n", __func__, digest));
	return true;
}


