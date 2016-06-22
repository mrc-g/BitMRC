/*
 * Storable.h
 *
 *  Created on: 07.06.2016
 *      Author: steady286
 *      provide a baseclass for all object needed to be stored into
 *      some storage backend.
 *      storables must be searchable.
 */

#ifndef STORABLE_H_
#define STORABLE_H_

#include <string>
#include <Storage/Unique_Key.h>

enum Storable_Type { STORABLE_NONE, STORABLE_TEST, STORABLE_ADDRESS, STORABLE_NODE, STORABLE_MESSAGE, STORABLE_KEY };

class Storable {
public:
	Storable() : type(STORABLE_NONE) {};
	~Storable() {};
public:
	virtual  Unique_Key calc_key() = 0;
	virtual  bool query(Unique_Key &uq_key_in, string & data_out) = 0;
	virtual  bool store(Storable & object_in, Unique_Key & key_out) = 0;
	virtual  bool delete_storable(Storable & object_in) = 0;
	virtual  bool delete_storable(Unique_Key & key_in) = 0;
	virtual Storable & find_by_key(Unique_Key &) = 0;
public:
	uint16_t type;
	uint64_t generation_time;
};


#endif /* STORABLE_H_ */
