/*
 * Storage_sqlite3.h
 *
 *  Created on: 01.06.2016
 *      Author: steady286
 */
#include <sqlite3.h>
#include <Storage/Storable.h>
#ifndef STORAGE_SQLITE3_H_
#define STORAGE_SQLITE3_H_


/* when tests are enabled, expose internal functions */
#ifdef ENABLE_TESTS
#define PRIVATE
#else
#define PRIVATE static
#endif

using namespace std;

enum { key_unused = 0, key_used = 1 } enKeyFlags;
enum { 	create_table = 1, insert_data = 2,
		query_system = 3 , update_insert_delete = 4 } enSQL3QueryType;

struct querytype {
	const char query[256];
};

class Storage_sqlite3 {
private:
	static int q_callback (void* param ,int count, char** keys, char** values);
public:
	int is_sqlite_error(int errin);
	void close();
	Storage_sqlite3();
	~Storage_sqlite3();
	bool open(std::string path, std::string user, std::string password);
	bool close(Storage & obj_in);
	void flush(Storage & obj_in);
	bool register_storable(Storable * object);
	uint32_t sql_exec(uint16_t type, const char * query);
private:/** \todo: implement this as storable data */
	bitmrc_sysinfo_t m_sinfo;
public: /** \todo: implement this as storable callback */
	uint32_t query_system_table(bitmrc_sysinfo_t * s_info);
	uint32_t create_tables();
	uint32_t populate_system_table();

private:
	uint32_t init();
#ifdef ENABLE_TESTS
public:
	uint32_t init(const unsigned char * db_path);
#endif
private:
	sqlite3 *db;
	mutable shared_timed_mutex mutex;
	static vector<Storable *> storable_list;
	std::string db_name;
	std::string db_user;
	std::string db_passwd;
	int init_complete;
	uint32_t last_error;
};
#endif /* STORAGE_SQLITE3_H_ */
