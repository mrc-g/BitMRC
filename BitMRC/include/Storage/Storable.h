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
#ifdef LINUX
#include <sys/time.h>
#endif
#include <string>
#include <Storage/Unique_Key.h>

#ifdef DEBUG_STORABLES
#define STORABLE_DEBUG(a) printf a;
#else
#define STORABLE_DEBUG(a)
#endif

enum Storable_Type { STORABLE_NONE, STORABLE_TEST, STORABLE_PUBADDRESS, STORABLE_PRIVADDR, STORABLE_NODE, STORABLE_MESSAGE, STORABLE_KEY, STORABLE_SETTINGS, STORABLE_MAX};

class Storable {
public:
	Storable();
	~Storable();
public:
	Unique_Key calc_key();
	virtual  bool query(Unique_Key &uq_key_in, std::string & data_out) = 0;
	virtual  bool store(Storable & object_in, Unique_Key & key_out) = 0;
	virtual  bool delete_storable(Storable & object_in) = 0;
	virtual  bool delete_storable(Unique_Key & key_in) = 0;
	virtual Storable & find_by_key(Unique_Key &) = 0;
protected:
	uint16_t type;
	uint64_t generation_time;
};
#if 0

Unique_Key StorableTest::calc_key() {
	Unique_Key uq;
	unsigned char key_string[120];
	// SHA3_256 sha;

	std::string str;
	if(generation_time == 0) {
		/* if no gentime is set, do not use default value for generation as this would
		 * create only one value */
		struct timeval tv = {.0};
		gettimeofday(&tv, NULL);
		this->generation_time = tv.tv_sec * 1000 * 1000 + tv.tv_usec;
	}
	sprintf(key_string, "%05hu%llu", this->type, this->generation_time);
	str.assign(key_string);
	uq.set_key(str);

	STORABLE_DEBUG(("KEY is %s\n", key_string));
	return uq;
}
bool StorableTest::query(Unique_Key &uq_key_in, string & data_out) {
	return true;
}
bool StorableTest::store(Storable & object_in, Unique_Key & key_out) {
	return true;
}
bool StorableTest::delete_storable(Storable & object_in) {
	return true;
}
bool StorableTest::delete_storable(Unique_Key & key_in) {
	return true;
}
Storable & StorableTest::find_by_key(Unique_Key & key_in) {
	StorableTest val;
	return val;
}

#endif

#endif /* STORABLE_H_ */
