/*
 * Storage_sqlite3.h
 *
 *  Created on: 01.06.2016
 *      Author: steady286
 */
#ifndef STORAGE_SQLITE3_H_
#define STORAGE_SQLITE3_H_

#include <sqlite3.h>
#include <Storage/Storable.h>
#include <Storage/Storage.h>
#include <Storage/StorableQueryConfig.h>
#include <types.h>
#include <shared_mutex>

using namespace std;

#if 0
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
#endif
class Storage_sqlite3 : public Storage {
private:
	static int q_callback (void* param ,int count, char** keys, char** values);
protected:

	static sqlite3 * getDb();
public:
	int is_sqlite_error(int errin);
	bool close();
	Storage_sqlite3();
	~Storage_sqlite3();
	bool open(std::string path, std::string user, std::string password);
	bool close(Storage & obj_in);
	void flush(Storage & obj_in);
	bool register_storable(Storable * object);
	QueryResult * sql_exec(StorableQueryConfig*);
	bool prepareResultRow(sqlite3_stmt *pStmt, QueryResult * qr);
private:/** \todo: implement this as storable data */
	bitmrc_sysinfo_t m_sinfo;
public: /** \todo: implement this as storable callback */
	uint32_t query_system_table(bitmrc_sysinfo_t * s_info);
	uint32_t create_tables();
	uint32_t populate_system_table();
	static sqlite3 *db;
private:
	uint32_t init();
#ifdef ENABLE_TESTS
public:
	uint32_t init(const char * db_path);
#endif
private:
	sqlite3_stmt * pStmt;
	mutable std::shared_timed_mutex mutex;
	static vector<Storable *> storable_list;
	std::string db_name;
	std::string db_user;
	std::string db_passwd;
	int init_complete;
	uint32_t last_error;
};
#endif /* STORAGE_SQLITE3_H_ */
