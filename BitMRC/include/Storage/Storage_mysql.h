/*
 * Storage_mysql.h
 *
 *  Created on: 22.12.2016
 *      Author: steady286
 */
#ifndef STORAGE_MYSQL_H_
#define STORAGE_MYSQL_H_

#ifdef LINUX
#include <mysql/mysql.h>
#else
#include <mysql.h>
#endif

#include <Storage/Storable.h>
#include <Storage/Storage.h>
#include <Storage/StorableQueryConfig.h>
#include <types.h>
#include <shared_mutex>

using namespace std;

class Storage_mysql : public Storage {
protected:
	static MYSQL* getDb();
public:
	Storage_mysql();
	~Storage_mysql();
public:
	int is_sqlite_error(int errin);
	bool open(std::string path, std::string user, std::string password);
	bool close();
	void flush(Storage & obj_in);
	bool register_storable(Storable * object);
	QueryResult * sql_exec(StorableQueryConfig*);

private:/** \todo: implement this as storable data */
	bitmrc_sysinfo_t m_sinfo;
public: /** \todo: implement this as storable callback */
	uint32_t query_system_table(bitmrc_sysinfo_t * s_info);
	uint32_t create_tables();
	uint32_t populate_system_table();
	uint32_t get_row_count();
	bool prepareResultRow(MYSQL_RES *res, QueryResult * qr);
	static MYSQL *db;
private:
	uint32_t init();
#ifdef ENABLE_TESTS
public:
	bool drop_tables();
	uint32_t init(const char * db_path);
#endif
private:
	mutable std::shared_timed_mutex mutex;
	static vector<Storable *> storable_list;
	int init_complete;
	std::string db_name;
	std::string db_user;
	std::string db_passwd;
	uint32_t row_count;
	uint32_t last_error;

};
#endif /* STORAGE_SQLITE3_H_ */
