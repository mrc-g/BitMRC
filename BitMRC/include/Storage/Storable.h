/*
 * Storable.h
 *
 *  Created on: 07.06.2016
 *      Author: steady286
 */

#ifndef STORABLE_H_
#define STORABLE_H_

#include <string>
#include <Storage/Unique_Key.h>

class Storable {
public:
	Storable() {};
	~Storable() {};
public:
	virtual  Unique_Key calc_key(Storable & object_in) = 0;
	virtual  bool query(Unique_Key &uq_key_in, string & data_out) = 0;
	virtual  bool store(Storable & object_in, Unique_Key & key_out) = 0;
	virtual  bool delete_storable(Storable & object_in) = 0;
	virtual  bool delete_storable(Unique_Key & key_in) = 0;
	virtual Storable & 	find_by_key(Unique_Key &) = 0;
};


#endif /* STORABLE_H_ */
