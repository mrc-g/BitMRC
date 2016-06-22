/*
 * Unique_Key.cpp
 *
 *  Created on: 01.06.2016
 *      Author: steady286
 */
#include <Addr.h>
#include <Storage/Unique_Key.h>
#include <string>

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
	this->key = key_in;
	return true;
}


