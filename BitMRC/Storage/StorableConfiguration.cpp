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
 * StorableConfigration.cpp
 *
 *  Created on: 28.12.2016
 *      Author: steady286
 *      bitmrc configuration load/save etc.
 *
 *      use
 *      $> sqlite3 storabletest.sqlite3
 *      sqlite> .dump
 *      for sqlite, or
 *
 *      $> mysqldump -d bitmrc -u bitmrc -p
 *      for mysql
 *
 *      to view the db.
 */
#include <cpptest.h>
#include <Storage/Storable.h>
#include <Storage/Storage.h>
#include <Storage/StorableConfiguration.h>
#include <Storage/StorableQueryConfig.h>
#include <Storage/Storage_sqlite3.h>
#include <Storage/Storage_mysql.h>
#include <Storage/RowSpec.h>
#include <Configuration.h>
#include <sys/time.h>
#include <sha3.h>
#include <utils.h>
#include <Addr.h>
using namespace std;

StorableConfiguration::StorableConfiguration()
	: Storable(), Configuration(),strg_sqlite3(NULL), strg_mysql(NULL)
{

}
StorableConfiguration::StorableConfiguration(Storage * strg_in)
	: strg_sqlite3(NULL), strg_mysql(NULL)
{
		strg = strg_in;


}

StorableConfiguration::~StorableConfiguration() {
	if(strg_sqlite3 != NULL)
		strg_sqlite3->close();
	if(strg_mysql != NULL)
		strg_mysql->close();
}

bool StorableConfiguration::query(Unique_Key &uq_key_in, std::string & data_out) {
	return false;
}

bool StorableConfiguration::set_storage(Storage* st_in) {
	strg = st_in;
	strg_sqlite3 = dynamic_cast<Storage_sqlite3*>(st_in);
	strg_mysql = dynamic_cast<Storage_mysql*>(st_in);
	return true;
}

Unique_Key StorableConfiguration::calc_key() {
	static char digest[] = {"DEFAULT_CONFIG"};
	Unique_Key uq;
	uq.set_digest(digest);
	return uq;
}
bool StorableConfiguration::store() {
	Unique_Key out = calc_key();
	char * query = get_update_query();
	QueryResult * qr = NULL;
	uint32_t ui_out ;
	bool bret = false;

	StorableQueryConfig q_cfg(SQC_UPDATE, query, 1, NULL);
	strg->sql_exec(&q_cfg);
	if ( q_cfg.getBitMRCError() == BITMRC_OK) {
		qr = q_cfg.getQueryResult();
		if (qr) {
			bret = qr->getFieldUint32("database_version", &ui_out);
		}
	}
	return bret;
}
#if 0
bool store(Storable & object_in, Unique_Key & key_out) {
	return true;
}

bool StorableConfiguration::load(Storable & obj, Unique_Key & key_out) {
	cout << "** load query :" << get_load_query() << endl;
	StorableQueryConfig cfg(this->type, 1, get_load_query(), NULL);
	strg->sql_exec(&cfg);
	return true;
}
#endif

bool StorableConfiguration::delete_storable(Storable & object_in) {
	return true;
}

bool StorableConfiguration::delete_storable(Unique_Key & key_in) {
	return true;
}

bool StorableConfiguration::find_by_key(Unique_Key & key_in) {

	char query[256] ;
	bool ret = false;
	sprintf(query, "select node_id, working_mode, networking_flags, stream_id1, stream_id2, stream_id3, stream_id4, "
			"last_startup_timestamp, last_startup_result, database_version from system where config_name = \"%s\"" , key_in.get_key().c_str());

	QueryResult * qr = NULL;
	StorableQueryConfig cfg(SQC_SELECT, query, 1, NULL);
	cfg.setQuery(query);

	qr = strg_mysql->sql_exec(&cfg);
	if (strg_mysql->get_row_count()>0) {
		ret = true;;
	}
	return ret;
}
#if 0
void StorableConfiguration::CalcKey() {
	Unique_Key uqk = this->calc_key();
	if(uqk.get_digest_len() != 16) {
		cout << "digest len " << uqk.get_digest_len() << endl;
	}
}
#endif
char * StorableConfiguration::get_update_query() {
	if(strg_sqlite3 != NULL)
		return get_update_query_sqlite3();
	if(strg_mysql != NULL)
		return get_update_query_mysql();
	return NULL;
}
char * StorableConfiguration::get_load_query() {
	if(strg_sqlite3 != NULL)
		return get_load_query_sqlite3();
	if(strg_mysql != NULL)
		return get_load_query_mysql();
	return NULL;
}

char * StorableConfiguration::get_update_query_mysql() {
	static char query[512];

	sprintf(query, "call save_system_table (%u,%u,%u,%u,%u,%u,%u,%u,%u, \"DEFAULT_CONFIG\" );",
			this->get_node_id(), this->get_working_mode(), this->get_networking_flags(),
			this->get_stream_id1(), this->get_stream_id2(), this->get_stream_id3(), this->get_stream_id4(),
			this->get_last_startup_result(), this->get_database_version());
	return query;
}
char * StorableConfiguration::get_update_query_sqlite3() {
	static char query[512];

	sprintf(query, "insert into system "
			"(node_id, working_mode, networking_flags, stream_id1, stream_id2, stream_id3, stream_id4, last_startup_timestamp, last_startup_result, database_version,config_name) "
			"values (%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,\"DEFAULT_CONFIG\");",
			this->get_working_mode(), this->get_networking_flags(), this->get_stream_id1(), this->get_stream_id2(),
			this->get_stream_id3(), this->get_stream_id4(),
			this->get_last_startup_time(), this->get_last_startup_result(),
			this->get_database_version());
	return query;
}

char * StorableConfiguration::get_load_query_mysql() {
	static char query[256];
	sprintf(query, "select node_id, working_mode, networking_flags, stream_id1, stream_id2, stream_id3, stream_id4, last_startup_timestamp, last_startup_result, database_version from system where config_name=\"DEFAULT_CONFIG\"	");
	return query;
}
char * StorableConfiguration::get_load_query_sqlite3() {
	return get_load_query_mysql();
}
