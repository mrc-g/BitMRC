/*
 * Storage_sqlite3.cpp
 *
 *  Created on: 28.05.2016
 *      Author: steady286
 *
 *      Storage backend for sqlite3 flat file db
 *
 */
#include <Storage/StorageInterface.h>
#include <Storage/Storable.h>
#include <Storage/Storage.h>
#include <Storage/Storage_sqlite3.h>
#include <types.h>

#include <vector>
#include <iostream>
#include <mutex>
#include <stdint.h>
#include <sqlite3.h> /** need to use: packages libsqlite3-dev and sqlite3 on linux */

vector<Storable *> Storage_sqlite3::storable_list;
static insert_node_return_t in;

Storage_sqlite3::Storage_sqlite3() {
	init_complete = 0;
	last_error = BITMRC_OK;
	storable_list.clear();
	db_name.clear();
	db_user.clear();
	db_passwd.clear();
	m_sinfo = {};
}
/** \brief helper for determining if passed int value is an sqlite3 error code
 *
 */
int Storage_sqlite3::is_sqlite_error(int errin) {

	return 0;
}
/** \brief the needed callback function
 * \todo: generalise
 */
int Storage_sqlite3::q_callback(void* param, int count, char** keys, char** values) {
    LOG_DB(("%s param %p columns: %d\n",__func__, param, count));
	/* \todo: unify */
    if(param == NULL)
    	return 1;
    query_config_t *cfg = (query_config_t *)param;
 	if ( cfg->query_type == create_table ) {
		LOG_DB((" create tables\n"));
 	} else if (cfg->query_type == query_system) {
 		/* system table has been queried */
 		bitmrc_sysinfo_t *si = (bitmrc_sysinfo_t*)cfg->data_struct_ptr;

 		while(count>0) {
 			LOG_DB((" system table:%s == %s\n", values[count-1], keys[count-1]));
 			count--;
 		}
 		si->node_id = (uint64_t)atoi(keys[0]);
 		si->database_version = (uint16_t)atoi(keys[9]);
#if 0
 		si->node_id = (uint64_t)atoi(keys[0]);
 		si->working_mode = atoi(keys[1]);
 		si->networking_flags = atoi(keys[2]);
 		si->stream_ids[0] = (uint16_t)atoi(keys[3]);
 		si->stream_ids[1] = (uint16_t)atoi(keys[4]);
 		si->stream_ids[2] = (uint16_t)atoi(keys[5]);
 		si->stream_ids[3] = (uint16_t)atoi(keys[6]);
 		si->last_startup_timestamp = atoi(keys[7]);
 		si->last_startup_result = atoi(keys[8]);
 		si->database_version = (uint16_t)atoi(keys[9]);
#endif
 		// LOG_DB(("node_id %llu db_version %hu\n", si->node_id, si->database_version));
	} else if ( cfg->query_type == insert_data) {

		/* node storable has been stored , last_insert_id() returned as only field
		in.last_insert_id = (uint32_t)atoi(keys[0]);*/

		LOG_DB((" inserted data id\n"));
	}
	return 0;
}
void Storage_sqlite3::close() {
	if (db!=NULL)
		sqlite3_close(db);
}

/** \brief init the client data module
 * this also includes the sqlite-backend used
 */
uint32_t Storage_sqlite3::init() {
	db = NULL;

	LOG_DB(("%s : open db %s\n",__func__ , db_name.c_str()));
	if ( sqlite3_initialize() != SQLITE_OK )
		return BITMRC_CLIENT_DB_INIT_BAD;
	if( sqlite3_open(db_name.c_str(), &db) == SQLITE_CANTOPEN) {
		return BITMR_DB_OPEN_FAILED;
	}
	init_complete = 1;
	LOG_DB(("%s open db ok\n", __func__));
	return BITMRC_OK;
}
#ifdef ENABLE_TESTS
/** \brief init functions for test mode
 *
 */
uint32_t Storage_sqlite3::init(const unsigned char * db_path) {
	db = NULL;

	LOG_DB(("%s : open db %s\n",__func__ , db_path));
	if ( sqlite3_initialize() != SQLITE_OK )
		return BITMRC_CLIENT_DB_INIT_BAD;
	if( sqlite3_open(db_path,&db) !=0 ) {
		return BITMR_DB_OPEN_FAILED;
	}
	init_complete = 1;
	LOG_DB(("%s open db ok\n", __func__));
	return BITMRC_OK;
}
#endif
/** \brief
 */
Storage_sqlite3::~Storage_sqlite3() {
	cout << "Storage_sqlite3 destructor" << endl;
}

/** \brief open the database with given arguments
 * for the sqlite3 type this is just the path, user information is not needed
 * set the last_error member according to the result.
 * \return true if everything is okay
 * \return false if there was an error
 */
bool Storage_sqlite3::open(std::string path, std::string user, std::string password) {
	db_name = path;
	db_user = user;
	db_passwd = password;
	if ((last_error = init()) == BITMRC_OK)
		return true;
	return false;
}
bool Storage_sqlite3::close(Storage & obj_in) {
	return true;
}
void Storage_sqlite3::flush(Storage & obj_in) {

}
bool Storage_sqlite3::register_storable(Storable * object) {
	// add the ref to the vector for further use. all storables using the storage must
	// have been registered.
	LOG_DB(("%s \n", __func__));

	return true;
}

uint32_t Storage_sqlite3::sql_exec(uint16_t type, const char * query) {
	uint32_t ret = BITMRC_SOME_ERROR;
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"
	void * x_param = (void*)type;
#pragma GCC diagnostic warning "-Wint-to-pointer-cast"
	char * errinfo = NULL;
	int sret = sqlite3_exec(db, query, q_callback, x_param, &errinfo);
	if((is_sqlite_error(sret)) || errinfo != NULL ) {
		ret = BITMRC_DB_EXEC_FAILED;
		LOG_DB(("%s : sqlite3 exec %d error %s\n",__func__, sret, errinfo));
		sqlite3_free(errinfo);
	} else {
		ret = BITMRC_OK;
	}
	return ret;
}

/** these functions have to be implemented as "callbacks" inside the storable - objects
 *
 */

/** \brief query the system table for settings
 */
uint32_t Storage_sqlite3::query_system_table(bitmrc_sysinfo_t * s_info) {
	uint32_t ret = BITMRC_OK;
	char * errinfo = NULL;
	if (s_info == NULL) {
		return BITMRC_BAD_PARA;
	}
	query_config_t cfg; cfg.query_type = query_system; cfg.data_struct_ptr = s_info ;

	/* keep track of last startup, so re-start loops can be avoided */
	int sret = sqlite3_exec(db, "select node_id, working_mode, networking_flags,stream_id1, stream_id2, stream_id3, stream_id4, last_startup_timestamp, last_startup_result, database_version from system;",
				q_callback,(void*)&cfg, &errinfo);
	if((is_sqlite_error(sret)) || errinfo != NULL ) {
		if (strcmp("no such table",errinfo ) == 0) {
			ret = BITMRC_DB_NOT_FOUND;
		} else {
			ret = BITMRC_DB_EXEC_FAILED;
		}
		LOG_DB(("%s : sqlite3 exec error  %d (%s)\n",__func__ , sret, errinfo));
		sqlite3_free(errinfo);
	} else {
		ret = BITMRC_OK;
	}
	last_error = ret;
	return ret;
}
/** \brief query the system table for settings
 */
uint32_t Storage_sqlite3::create_tables() {
	uint32_t ret = BITMRC_OK;
	int index = 0;
	char * errinfo = NULL;

	/* these table structures have  been taken from pybitmessage 0.5.8 */
	struct querytype qt[] = {
	"CREATE TABLE inbox (msgid blob, toaddress text, fromaddress text, subject text, received text, message text, folder text, encodingtype int, read bool, sighash blob, UNIQUE(msgid) ON CONFLICT REPLACE);",
	"CREATE TABLE sent (msgid blob, toaddress text, toripe blob, fromaddress text, subject text, message text, ackdata blob, senttime integer, lastactiontime integer, sleeptill integer, status text, retrynumber integer, folder text, encodingtype int, ttl int);",
	"CREATE TABLE subscriptions (label text, address text, enabled bool);",
	"CREATE TABLE addressbook (label text, address text);",
	"CREATE TABLE blacklist (label text, address text, enabled bool);",
	"CREATE TABLE whitelist (label text, address text, enabled bool);",
	"CREATE TABLE pubkeys (address text, addressversion int, transmitdata blob, time int, usedpersonally text, UNIQUE(address) ON CONFLICT REPLACE);",
	"CREATE TABLE inventory (hash blob, objecttype int, streamnumber int, payload blob, expirestime integer, tag blob, UNIQUE(hash) ON CONFLICT REPLACE);",
	"CREATE TABLE settings (key blob, value blob, UNIQUE(key) ON CONFLICT REPLACE);",
	"CREATE TABLE objectprocessorqueue (objecttype int, data blob, UNIQUE(objecttype, data) ON CONFLICT REPLACE);" ,
	/* bitmrc additions */
	"CREATE TABLE system (node_id int, working_mode int, networking_flags int, stream_id1 int, stream_id2 int, stream_id3 int, stream_id4 int, last_startup_timestamp int, last_startup_result int, database_version int);",
	"\0" };
	int sret;
	query_config_t cfg; cfg.query_type = create_table; cfg.data_struct_ptr = NULL;

	while(strlen(qt[index].query)>0) {
		/* keep track of last startup, so re-start loops can be avoided */
		sret = sqlite3_exec(db, qt[index].query, q_callback, (void*)&cfg, &errinfo);
		if((is_sqlite_error(sret)) || errinfo != NULL ) {
			LOG_DB(("%s : sqlite3 exec error  %d (%s)\n",__func__ , sret, errinfo));
			sqlite3_free(errinfo);
			ret = BITMRC_TABLE_CREATE_FAILED;
		}
		index++;
	}
	last_error = ret;
	return ret;
}
/** \brief this sets the BitMRC system values like db-version
 * in the db-table "system". We store version-relevant data, so we can provide automatic db
 * updates more easily
 */
uint32_t Storage_sqlite3::populate_system_table() {
	uint32_t ret = BITMRC_OK;
	int index = 0, sret;
	char * errinfo = NULL;
	/* pass the type of query plus additional data ptr to query for use in q_callback */
	query_config_t cfg; cfg.query_type = insert_data, cfg.data_struct_ptr = NULL;

	struct querytype qt[] = {
	 {"insert into system (node_id, working_mode, networking_flags, database_version) values (1000,1,1,1);"},
	 {"INSERT INTO subscriptions VALUES('Bitmessage new releases/announcements','BM-GtovgYdgs7qXPkoYaRgrLFuFKz1SFpsw',1);"},
	 {"INSERT INTO subscriptions VALUES('BitMRC Development Channel','BM-2cVxGMPpzu1WwnpUwAvcy9aQpuS9deouky',1);"}
	};
	/* first, do completed queries */
	while(strlen(qt[index].query)>0) {
		/* keep track of last startup, so re-start loops can be avoided */
		sret = sqlite3_exec(db, qt[index].query, q_callback, (void*)&cfg, &errinfo);
		if((is_sqlite_error(sret)) || errinfo != NULL ) {
			ret = BITMRC_DB_EXEC_FAILED;
			LOG_DB(("%s : sqlite3 exec error  %d (%s)\n",__func__ , sret, errinfo));
			sqlite3_free(errinfo);
			last_error = ret;
		}
		index++;
	}
	/* insert settings from keys.dat */
	struct settingstype st[] = {
			{{"settingsversion"},{"10"}},
			{{"port"},{"8444"}},
			{{"timeformat"},{"%%a, %%d %%b %%Y  %%I:%%M %%p"}},
			{{"blackwhitelist"},{"black"}},
			{{"startonlogon"},{"False"}},
			{{"minimizetotray"},{"True"}},
			{{"showtraynotifications"},{"True"}},
			{{"startintray"},{"False"}},
			{{"socksproxytype"},{"none"}},
			{{"sockshostname"},{"localhost"}},
			{{"socksport"},{"9050"}},
			{{"socksauthentication"},{"False"}},
			{{"sockslisten"},{"False"}},
			{{"socksusername"},{""}},
			{{"sockspassword"},{""}},
			{{"keysencrypted"},{"false"}},
			{{"messagesencrypted"},{"false"}},
			{{"defaultnoncetrialsperbyte"},{"1000"}},
			{{"defaultpayloadlengthextrabytes"},{"1000"}},
			{{"minimizeonclose"},{"false"}},
			{{"maxacceptablenoncetrialsperbyte"},{"20000000000"}},
			{{"maxacceptablepayloadlengthextrabytes"},{"20000000000"}},
			{{"userlocale"},{"de"}},
			{{"useidenticons"},{"True"}},
			{{"identiconsuffix"},{"pZByMut5ZkZN"}},
			{{"replybelow"},{"False"}},
			{{"maxdownloadrate"},{"250"}},
			{{"maxuploadrate"},{"75"}},
			{{"stopresendingafterxdays"},{"21.0"}},
			{{"stopresendingafterxmonths"},{"0.0"}},
			{{"namecoinrpctype"},{"namecoind"}},
			{{"namecoinrpchost"},{"localhost"}},
			{{"namecoinrpcuser"},{""}},
			{{"namecoinrpcpassword"},{""}},
			{{"namecoinrpcport"},{"8336"}},
			{{"sendoutgoingconnections"},{"True"}},
			{{"willinglysendtomobile"},{"False"}},
			{{"ttl"},{"417641"}},
			{{"trayonclose"},{"False"}},
			{{"version"},{"10"}},
			{{"lastvacuumtime"},{"?"}}, {{"\0"}, {"\0"}}
			};
	/* then: create queries for inserting settings and execute */
	while(strlen(st[index].key)>0) {
		unsigned char insert_query[256];
		sprintf(insert_query, "insert into settings (key,value) values ('%s','%s');",st[index].key, st[index].value);
		sret = sqlite3_exec(db, insert_query, q_callback, (void*)&cfg, &errinfo);
		if((is_sqlite_error(sret)) || errinfo != NULL ) {
			ret = BITMRC_DB_EXEC_FAILED;
			LOG_DB(("%s : sqlite3 exec [%s] error  %d (%s)\n",__func__ , insert_query, sret, errinfo));
			sqlite3_free(errinfo);
			last_error = ret;
		}
		index++;
	}
	return ret;
}
