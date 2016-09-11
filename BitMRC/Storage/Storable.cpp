/*
 * Storable.cpp
 *
 *  Created on: 09.06.2016
 *      Author: steady286
 */
#include <Storage/Storable.h>
#include <utils.h>

Storable::Storable() {
#ifdef LINUX
	struct timeval tv = {.0};
	gettimeofday(&tv, NULL);
	this->generation_time = tv.tv_sec * 1000 * 1000 + tv.tv_usec;
#else
	this->generation_time = 1451602800 ; /* 1.Jan.2016 00:00 */
	#error "fixme"
#endif
}
Storable::~Storable() {

}
Unique_Key Storable::calc_key() {
	Unique_Key uq;
	unsigned char key_string[120];
	std::string tmp_str;
	ustring str;

	tmp_str.clear();
	str.clear();
	/* key format : type and time so we can search for specific objects generated at certain times */
	sprintf(key_string, "%05hu%012llu", this->type, this->generation_time);
	tmp_str.assign(key_string);
	str.assign(key_string);
	uq.set_key(tmp_str);
#ifdef DEBUG_STORABLES
	std::string hex_str;
	hex_str.clear();
	hex_str = str.toHexString();
	STORABLE_DEBUG(("%s %s KEY is (%s) \n", __FILE__, __func__, key_string));
	STORABLE_DEBUG(("%s %s HEX is \n%s\n", __FILE__, __func__, hex_str.c_str()));
#endif
	return uq;
}
