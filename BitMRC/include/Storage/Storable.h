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
 * Storable.h
 *
 *  Created on: 07.06.2016
 *      Author: steady286
 *      provide a baseclass for all object needed to be stored into
 *      some storage backend.
 *      storables must be searchable.
 *
 *      Usage: implement all virtual methods, and overload "calc_key()" if you wish to have
 *      the global id (bitmessage object id) to be generated in a different way
 */

#ifndef STORABLE_H_
#define STORABLE_H_
#ifdef LINUX
#include <sys/time.h>
#endif
#include <string>
#include <Storage/Unique_Key.h>
#include <Storage/QueryResult.h>
class Storage;
#ifdef DEBUG_STORABLES
#define STORABLE_DEBUG(a) printf a;
#else
#define STORABLE_DEBUG(a)
#endif


enum Storable_Type { STORABLE_NONE, STORABLE_TEST, STORABLE_PUBADDRESS, STORABLE_PRIVADDR, STORABLE_NODE, STORABLE_MESSAGE, STORABLE_KEY, STORABLE_SETTINGS, STORABLE_CREATION, STORABLE_MAX};

class Storable {
public:
	Storable();
	Storable(Storage * strg);
	~Storable();
public:
	bool set_key(Unique_Key & key_in); // set the unique key manually
	Unique_Key get_key();
	virtual bool query(Unique_Key &uq_key_in, std::string & data_out) = 0;
	virtual bool store() = 0;
	virtual bool delete_storable(Storable & object_in) = 0;
	virtual bool delete_storable(Unique_Key & key_in) = 0;
	virtual bool find_by_key(Unique_Key &) = 0;
	bool set_storage(Storage*);
	virtual char * get_update_query();
	virtual char * get_load_query();
protected:
	Unique_Key calc_key(); // implicitly called by constructors

protected:
	uint16_t type;
	uint64_t generation_time;
	Storage * strg;
	static char no_query[];

};


#endif /* STORABLE_H_ */
