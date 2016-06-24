/*
 * Storage_sqlite3.h
 *
 *  Created on: 01.06.2016
 *      Author: steady286
 */
#include <sqlite3.h>

#ifndef STORAGE_SQLITE3_H_
#define STORAGE_SQLITE3_H_

using namespace std;

class Storage_sqlite3 {
private:
	int q_callback (void* param ,int count, char** keys, char** values);
public:
	int is_sqlite_error(int errin);
	void close();
	uint32_t init();
	Storage_sqlite3();
	~Storage_sqlite3();
	bool open(std::string path, std::string user, std::string password);
	bool close(Storage & obj_in);
	void flush(Storage & obj_in);
	bool register_storable(Storable * object);
private:
	sqlite3 *db;
	mutable shared_timed_mutex mutex;
	static vector<Storable *> storable_list;


};
#endif /* STORAGE_SQLITE3_H_ */
