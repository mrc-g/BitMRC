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
#include <Storage/StorableQueryConfig.h>
#include <types.h>

#include <vector>
#include <iostream>
#include <mutex>
#include <stdint.h>
#include <sqlite3.h> /** need to use: packages libsqlite3-dev and sqlite3 on linux */
#include <Exceptions/BitMRC_NoQueryConfigException.h>

vector<Storable *> Storage_sqlite3::storable_list;
static insert_node_return_t in;
sqlite3 * Storage_sqlite3::db = NULL;

Storage_sqlite3::Storage_sqlite3() {
	init_complete = 0;
	last_error = BITMRC_OK;
	storable_list.clear();
	db_name.clear();
	db_user.clear();
	db_passwd.clear();
	m_sinfo = {};
	pStmt = NULL;
}
/** \brief helper for determining if passed int value is an sqlite3 error code
 *
 */
int Storage_sqlite3::is_sqlite_error(int errin) {

	if (errin == SQLITE_OK || errin == SQLITE_DONE)
		return 0;
	return 1;
}
void qre_fabric () {


}
sqlite3 * Storage_sqlite3::getDb() {
	return db;
}
/** \brief the needed query callback function
 * \todo: generalise, use the query config to build return
 */
int Storage_sqlite3::q_callback(void* param, int count, char** keys, char** values) {
	STORAGE_DBG(("%s param %p columns: %d\n",__func__, param, count));
	/* \todo: unify */
    if(param == NULL)
    	return 1;
    StorableQueryConfig * cfg = (StorableQueryConfig*)param;
    switch (cfg->getType()) {
   	   case STORABLE_NONE: LOG_DB((" STORABLE_NONE\n")); break;
   	   case STORABLE_TEST: LOG_DB((" STORABLE_TEST\n")); break;
   	   case STORABLE_PUBADDRESS: LOG_DB((" STORABLE_PUBADDRESS\n")); break;
   	   case STORABLE_PRIVADDR: LOG_DB((" STORABLE_PRIVADDR\n")); break;
   	   case STORABLE_NODE: LOG_DB((" STORABLE_NODE\n")); break;
   	   case STORABLE_MESSAGE:  LOG_DB((" STORABLE_MESSAGE\n")); break;
   	   case STORABLE_KEY: LOG_DB((" STORABLE_KEY\n")); break;
   	   case STORABLE_SETTINGS: LOG_DB((" STORABLE_SETTINGS\n")); break;
   	   case STORABLE_CREATION: LOG_DB(("STORABLE_CREATION")); break;
   	   default: break;
   } // is this of any use ?
   // bool bret = prepareResultRowCallback(getDb(), cfg->getQueryResult());
   // LOG_DB(("PrepRow result %d\n", bret));
   /* pull in all fields (queryResultElements) into one query result */
   int column = 0;
   for(; column < count; column++) {
	   STORAGE_DBG((" ++ field %d name %s\n", column, keys[column]));
   }


#if 0
 	if ( type == create_table ) {
		LOG_DB((" create tables\n"));
 	} else if (type == query_system) {
 		/* system table has been queried */

 		Configuration * cfg = new Configuration();
 		bitmrc_sysinfo_t *si = (bitmrc_sysinfo_t*)cfg->data_struct_ptr;
 		while(count>0) {
 			LOG_DB((" system table:%s == %s\n", values[count-1], keys[count-1]));
 			count--;
 		}
 		si->node_id = (uint64_t)atoi(keys[0]);
 		si->database_version = (uint16_t)atoi(keys[9]);
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
 		// LOG_DB(("node_id %llu db_version %hu\n", si->node_id, si->database_version));
	} else if ( type == insert_data) {

		/* node storable has been stored , last_insert_id() returned as only field
		in.last_insert_id = (uint32_t)atoi(keys[0]);*/

		STORAGE_DBG((" inserted data id\n"));
	} else {
		STORAGE_DBG((" query type %u\n", type));

	}
#endif

	return 0;
}
bool Storage_sqlite3::close() {
	if (db!=NULL) {
		sqlite3_close(db);
		db = NULL;
		return true;
	}
	return false;
}

/** \brief init the client data module
 * this also includes the sqlite-backend used
 */
uint32_t Storage_sqlite3::init() {
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
uint32_t Storage_sqlite3::init(const char * db_path) {
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
	Storage_sqlite3::storable_list.push_back(object);
	LOG_DB(("%s : elements in storable list now: %u\n",__func__, (unsigned int)storable_list.size()));
	//set_storables_storage(object);
	return true;
}
#if 0
bool prepareResultRowCallback(getDb(), cfg->getQueryResult()) {

}
#endif
bool Storage_sqlite3::prepareResultRow(sqlite3_stmt *pStmt, QueryResult * qr) {
	if(db == NULL || qr == NULL)
		return false;
	QueryResultElement * el = NULL;
	int i = 0, columns = 0, ctype = 0;
	columns = sqlite3_column_count(pStmt);
	// LOG_DB(("%s : Prep cols %u\n",__func__, columns));
	if(columns < 1)
		return false;
	while(i < columns) {
		ctype = sqlite3_column_type(pStmt, i);
		// LOG_DB(("%s : Prep col %u, type %u\n",__func__,  i, ctype ));
		switch (ctype) {
			case SQLITE_INTEGER:
				el = new QueryResultElement((int32_t)sqlite3_column_int(pStmt, i));
				break;
			case SQLITE_FLOAT:
				el = new QueryResultElement((double)sqlite3_column_double(pStmt, i));
				break;
			case SQLITE_BLOB:
			case SQLITE_NULL:
				qr->addElement(new QueryResultElement()); break;
			case SQLITE3_TEXT:
				el = new QueryResultElement(sqlite3_column_text(pStmt, i), (uint32_t)strlen(sqlite3_column_text(pStmt, i)));
				break;
			default:
				LOG_DB(("%s : default type %d\n",__func__, ctype ));
				break;
		}
		if(el != NULL) {
			el->setName(sqlite3_column_name(pStmt, i));
			qr->addElement(el);
		}
		el = NULL;
		i++;
	}
	return true;
}

QueryResult * Storage_sqlite3::sql_exec(StorableQueryConfig * cfg)
{
//	if (cfg == NULL)
//		throw BitMRC_NoQueryConfigException();
	QueryResult * ret = new QueryResult(cfg->getType());
	// \todo: if !ret ...
	char * errinfo = NULL;

	cfg->setQueryResult(ret);
	LOG_DB(("%s: sqlite3 exec '%s'\n",__func__, cfg->getQuery()));
	sqlite3_stmt * l_pStmt = NULL;
	char * pzTail = NULL;
	int rows = 0, sret = 0;
#if 0
	sret = sqlite3_exec(db, cfg->getQuery(), q_callback, (void*)&cfg, &errinfo);
	if((is_sqlite_error(sret)) ) {
		cfg->setBitMRCError(BITMRC_DB_EXEC_FAILED);
		LOG_DB(("%s: sqlite3 exec %d ret:: %s %d\n",__func__, sret, sqlite3_errstr(sret), sqlite3_extended_errcode(db)));
	} else {
		LOG_DB(("%s: sqlite3 stmt exec okay\n",__func__));
		ret = cfg->getQueryResult();
		cfg->setBitMRCError(BITMRC_OK);
	}
	if(errinfo != NULL) {
		sqlite3_free(errinfo);
	}
	LOG_DB(("%s: sqlite3 errinfo %s\n",__func__, errinfo));
#endif

	/* (cfg->getType() != SQC_UPDATE && cfg->getType() != SQC_INSERT) */
	/* currently implementing... use ColumnSpec/Rowspec in Storable to generate the answer automatically */
	sret = sqlite3_prepare_v2(db, cfg->getQuery(), -1, &l_pStmt, &pzTail);
	if (sret == SQLITE_OK) {
		sret = sqlite3_step(l_pStmt);
		while(sret == SQLITE_ROW) {
			bool bret = prepareResultRow(l_pStmt, ret);
			LOG_DB(("%s: PrepResultRow, prep return %u\n",__func__, bret));
			sret = sqlite3_step(l_pStmt);
			rows++;
		}
		if (sret == SQLITE_DONE && rows == 0) {
			LOG_DB(("%s: insert query done..\n",__func__));
		} else {
			LOG_DB(("%s: query != DONE\n",__func__));
		}
		sqlite3_clear_bindings(l_pStmt);
	}
	if((is_sqlite_error(sret)) || l_pStmt == NULL ) {
		cfg->setBitMRCError(BITMRC_DB_EXEC_FAILED);
		LOG_DB(("%s: sqlite3 exec %d ret:: %s %d\n",__func__, sret, sqlite3_errstr(sret), sqlite3_extended_errcode(db)));
	} else {
		LOG_DB(("%s: sqlite3 stmt exec okay\n",__func__));
		ret = cfg->getQueryResult();
		cfg->setBitMRCError(BITMRC_OK);
	}
	if (l_pStmt) {
		sqlite3_reset(l_pStmt);
		sqlite3_finalize(l_pStmt);
	}
	l_pStmt = NULL;
	while ((l_pStmt = sqlite3_next_stmt(db, l_pStmt))!=NULL) {
		sqlite3_finalize(l_pStmt);
	}
	LOG_DB(("%s: exit. exec result %d\n",__func__, sret));
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
	char query[] = "select node_id, working_mode, networking_flags,stream_id1, stream_id2, stream_id3, stream_id4, last_startup_timestamp, last_startup_result, database_version from system;";
	StorableQueryConfig cfg(query_system, 1, query, NULL);

	/* keep track of last startup, so re-start loops can be avoided */
	int sret = sqlite3_exec(db, cfg.getQuery(), q_callback,(void*)&cfg, &errinfo);
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

	/* these table structures have been taken from pybitmessage 0.5.8 */
	struct querytype qt[] = {
	"CREATE TABLE inbox (msgid blob, toaddress text, fromaddress text, subject text, received text, message text, folder text, encodingtype int, read bool, sighash blob, UNIQUE(msgid) ON CONFLICT REPLACE);",
	"CREATE TABLE sent (msgid blob, toaddress text, toripe blob, fromaddress text, subject text, message text, ackdata blob, senttime integer, lastactiontime integer, sleeptill integer, status text, retrynumber integer, folder text, encodingtype int, ttl int);",
	"CREATE TABLE subscriptions (label text, address text, enabled bool);",
	"CREATE TABLE addressbook (label text, address text);",
	// todo:	"CREATE TABLE addressbook (label varchar(255), address varchar(255) not null, version int, stream int, last_request DATETIME, constraint c_uq_addr UNIQUE KEY (address));",
	"CREATE TABLE blacklist (label text, address text, enabled bool);",
	"CREATE TABLE whitelist (label text, address text, enabled bool);",
	// todo: 	"CREATE TABLE pubkeys (address varchar(255), addressversion int, signkey varchar(255), enckey varchar(255), time int, usedpersonally varchar(255), constraint c_uq_address UNIQUE KEY (address));",
	"CREATE TABLE pubkeys (address text, addressversion int, transmitdata blob, time int, usedpersonally text, UNIQUE(address) ON CONFLICT REPLACE);",
	"CREATE TABLE inventory (hash blob, objecttype int, streamnumber int, payload blob, expirestime integer, tag blob, UNIQUE(hash) ON CONFLICT REPLACE);",
	"CREATE TABLE settings (key_namel blob, val blob, UNIQUE(key_name) ON CONFLICT REPLACE);",
	"CREATE TABLE objectprocessorqueue (objecttype int, data blob, UNIQUE(objecttype, data) ON CONFLICT REPLACE);" ,
	/* bitmrc additions */
	"CREATE TABLE system (node_id int, working_mode int, networking_flags int, stream_id1 int, stream_id2 int, stream_id3 int, stream_id4 int, last_startup_timestamp int, last_startup_result int, database_version int, config_name text, UNIQUE(config_name) ON CONFLICT REPLACE);",
	"CREATE TABLE test_table_repl (intval int, stringval text, blobval blob,UNIQUE(intval) ON CONFLICT REPLACE);",
	"CREATE TABLE test_table_fail (intval int, stringval text, blobval blob,UNIQUE(intval) ON CONFLICT FAIL);",
	"\0" };
	int sret;
	QueryResult qr(QR_TYPE_CREATE);
	StorableQueryConfig cfg(STORABLE_CREATION, NULL, 1, &qr);
	cfg.setRowSpec( RowSpec() );

	while(strlen(qt[index].query)>0) {
		LOG_DB(("%s : sqlite3 exec (%s)\n",__func__ , qt[index].query ));
		cfg.setQuery(qt[index].query);
		sret = sqlite3_exec(db, qt[index].query, q_callback, (void*)&cfg, &errinfo);
		if((is_sqlite_error(sret)) || errinfo != NULL ) {
			sqlite3_free(errinfo);
			ret = BITMRC_DB_TABLE_CREATE_FAILED;
		}
		index++;
	}
	last_error = ret;
	return ret;
}
/** \brief this sets the BitMRC system values, such as 'db-version'
 * in the db-table "system".
 * We store version-relevant data, so we can provide automatic db
 * updates more easily
 */
uint32_t Storage_sqlite3::populate_system_table() {
	uint32_t ret = BITMRC_OK;
	int index = 0, sret;
	char * errinfo = NULL;
	/* pass the type of query plus additional data ptr to query for use in q_callback */

	struct querytype qt[] = {
	 {"insert into system (node_id, working_mode, networking_flags, database_version, stream_id1,stream_id2,stream_id3,stream_id4,config_name) values (1000,1,1,1,1,0,0,0,'DEFAULT_CONFIG');"},
	 {"INSERT INTO subscriptions VALUES('Bitmessage new releases/announcements','BM-GtovgYdgs7qXPkoYaRgrLFuFKz1SFpsw',1);"},
	 {"INSERT INTO subscriptions VALUES('BitMRC Development Channel','BM-2cVxGMPpzu1WwnpUwAvcy9aQpuS9deouky',1);"},
	 {"\0"}
	};

	QueryResult qr(QR_TYPE_CREATE);
	StorableQueryConfig cfg(STORABLE_CREATION, NULL, 1, &qr);
	cfg.setRowSpec( RowSpec() );

	/* first, do completed queries */
	while(strlen(qt[index].query)>0) {
		cfg.setQuery(qt[index].query);
		/* keep track of last startup, so re-start loops can be avoided */
		sret = sqlite3_exec(db, cfg.getQuery(), q_callback, (void*)&cfg, &errinfo);
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
		char insert_query[256];
		sprintf(insert_query, "insert into settings (key_name,val) values ('%s','%s');",st[index].key, st[index].value);
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
