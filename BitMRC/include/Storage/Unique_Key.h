/*
 * Unique_Key.h
 *
 *  Created on: 01.06.2016
 *      Author: steady286
 *      unique key class for use w/ Storage/Storable
 *
 *      we use generation time and type as input for generating the uqk, as
 *      these values are transmitted via the "getobject" result message
 */
#ifndef UNIQUE_KEY_H_
#define UNIQUE_KEY_H_


#include <stdlib.h>
#include <Storage/Storable.h>

class Addr;

const uint16_t digest_len_to_use = 64; /* bitmessage technical paper, pg5 : two rounds of sha512, use only the first 32 bytes of it */

class Unique_Key {


public:
	Unique_Key();
	~Unique_Key();
private:
	std::string key;
	size_t keylen;
	uint16_t digest_len;
	unsigned char digest[digest_len_to_use];
public:
	bool create(Addr &);
	bool operator==(Addr &);
	bool set_key(std::string key_in);
	uint16_t get_digest_len();
	std::string digest_to_string();
	void set_digest(const char in[digest_len_to_use]);
	std::string get_key();
};


#endif /* UNIQUE_KEY_H_ */
