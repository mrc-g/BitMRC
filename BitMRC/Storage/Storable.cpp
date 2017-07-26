/*
* Copyright (c) 2016 steady286 github@dutka-is.de
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
* OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
* OTHER DEALINGS IN THE SOFTWARE.
*/
/*
 * Storable.cpp
 *
 *  Created on: 09.06.2016
 *      Author: steady286
 */
#include <Storage/Storable.h>
#include <utils.h>
#include <iostream>

static char Storable::no_query[] = "undefined query";

Storable::Storable() {
#ifdef LINUX
	struct timeval tv = {0};
	gettimeofday(&tv, NULL);
	this->generation_time = (uint64_t)tv.tv_sec * 1000 * 1000 + tv.tv_usec;
#else
	struct timeval tv = { .0 };
	gettimeofday(&tv, NULL);
	this->generation_time = (uint64_t)tv.tv_sec * 1000 * 1000 + tv.tv_usec;
#endif
	this->strg = NULL;
	calc_key();
}
Storable::~Storable() {

}
Storable::Storable(Storage * strg) : Storable() {

}
bool Storable::set_key(Unique_Key & key_in) {

	return true;
}
Unique_Key Storable::get_key() {
	return this->calc_key();
}
Unique_Key Storable::calc_key() {
	Unique_Key uq;
	char key_string[120];
	std::string tmp_str;
	ustring str;
	tmp_str.clear();
	str.clear();
	/* key format : type and time so we can search for specific objects generated at certain times */
	sprintf(key_string, "%05hu%012lu", this->type, this->generation_time);
	tmp_str.assign(key_string);
	str.assign((unsigned char*)key_string);
	uq.set_key(tmp_str);
	uq.set_digest("EMPTY-DIGEST");

	/* to set a digest depending on the object type, overload this function, and set_digets().
	 * example code:
	unsigned char digest_round1[64];
	unsigned char digest_round2[64];
	SHA3 sha(64);
	sha.CalculateDigest(digest_round1, (const byte*)OBJECT, OBJECT_SIZE() );
	sha.CalculateDigest(digest_round2, (const byte*)digest_round1, uq.get_digest_len() );

	uq.set_digest(digest_round2);
	*/

	return uq;
}

/** \brief set the storage pointer.
 * this will be called by Storage at register_storable()
 */
bool Storable::set_storage(Storage * st) {
	this->strg = st;
}
char *  Storable::get_update_query() {
	return no_query;
}
char *  Storable::get_load_query() {
	return no_query;
}

