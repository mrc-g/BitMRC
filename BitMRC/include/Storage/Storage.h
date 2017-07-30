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
 * Storage.h
 *
 *  Created on: 07.06.2016
 *      Author: steady286
 */

#ifndef STORAGE_H_
#define STORAGE_H_

using namespace std;

#include <string.h>
#include <vector>
#include <Storage/Unique_Key.h>
#include <Storage/Storable.h>
#include <Storage/QueryResult.h>
#include <Storage/StorableQueryConfig.h>

#ifdef DEBUG_STORAGE
#define STORAGE_DBG(a) printf a;
#else
#define STORAGE_DBG(a)
#endif

#define TEST_DB_NAME "storabletest.sqlite3"
#define TEST_DB_NAME_MYSQL "bitmrc_test"
#define DB_NAME_MYSQL "bitmrc"

enum enKeyFlags { key_unused = 0, key_used = 1 };
enum enSQL3QueryType { 	create_table = 1, insert_data = 2,
		query_system = 3 , update_insert_delete = 4 };

struct querytype {
	char query[512];
};
struct settingstype {
	const char key[64];
	const char value[64];
};

typedef struct {
	uint32_t query_type;
	void * data_struct_ptr;
} query_config_t;

class Storage {
public:
	Storage() {};
	~Storage() {};
public:
	virtual bool register_storable(Storable * object) = 0;
public:
	// methods needed by all Storage objects
	virtual bool open(std::string path, std::string user, std::string password) = 0;
	virtual bool close() = 0;
	virtual void flush(Storage &) = 0;
	virtual QueryResult * sql_exec(StorableQueryConfig*) = 0;
protected:
	static std::vector<Storable *> storable_list ;

private:
};

#endif /* STORAGE_H_ */
