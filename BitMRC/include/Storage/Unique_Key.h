/*
 * Unique_Key.h
 *
 *  Created on: 01.06.2016
 *      Author: steady286
 *      unique key class for use w/ Storage/Storable
 */
#ifndef UNIQUE_KEY_H_
#define UNIQUE_KEY_H_

#include <Addr.h>

class Unique_Key {
public:
	Unique_Key();
	~Unique_Key();
private:
	std::string key;
public:
	bool create_from_addr(Addr &);
	bool operator==(Addr &);
	bool set_key(std::string key_in);
};



#endif /* UNIQUE_KEY_H_ */
