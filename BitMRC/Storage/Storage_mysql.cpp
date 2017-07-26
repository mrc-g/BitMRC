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
 * Storage_mysql.cpp
 *
 *  Created on: 22.12.2016
 *      Author: steady286
 *
 *      Storage backend for mysql database
 *
 */
#include <Storage/StorageInterface.h>
#include <Storage/Storable.h>
#include <Storage/Storage.h>
#include <Storage/Storage_mysql.h>
#include <Storage/StorableQueryConfig.h>
#include <types.h>

#include <vector>
#include <iostream>
#include <mutex>
#include <stdint.h>
#include <mysql/mysql.h>
//#include <Exceptions/BitMRC_NoQueryConfigException.h>

vector<Storable *> Storage_mysql::storable_list;
static insert_node_return_t in;
MYSQL * Storage_mysql::db = NULL;

Storage_mysql::Storage_mysql() : init_complete(0), last_error(BITMRC_OK) , row_count(0){
	storable_list.clear();
	db_name.clear();
	db_user.clear();
	db_passwd.clear();
	m_sinfo = {};
}
/** \brief
 */
Storage_mysql::~Storage_mysql() {
	cout << "Storage_mysql destructor" << endl;
}
/** \brief helper for determining if passed int value is a mysql error code
 *
 */
int Storage_mysql::is_sqlite_error(int errin) {

	//if (errin == MYSQL_OK)
		return 0;
	return 1;
}
MYSQL * Storage_mysql::getDb() {
	return db;
}

bool Storage_mysql::close() {
	if (db!=NULL) {
		mysql_close(db);
		db=NULL;
		return true;
	}
	return false;
}

/** \brief init the client data module
 *
 */
uint32_t Storage_mysql::init() {
	LOG_DB(("%s : open db %s\n",__func__, db_name.c_str()));
	MYSQL * ldb = NULL;
	db = mysql_init(NULL);

	ldb = mysql_real_connect(db, "localhost", db_user.c_str(), db_passwd.c_str(), db_name.c_str(), 0, "/var/run/mysqld/mysqld.sock", CLIENT_MULTI_RESULTS /* usage of sp ok */);

	LOG_DB(("  %s open db res : %p (%s)\n", __func__, ldb, mysql_error(db)));
	if(ldb == db)
	{
		return BITMRC_OK;
	}
	return BITMR_DB_OPEN_FAILED;
}
#ifdef ENABLE_TESTS
bool Storage_mysql::drop_tables() {
	bool ret = true;
	int index = 0;
	char * errinfo = NULL;

	/* these table structures have been taken from pybitmessage 0.5.8 */
	struct querytype qt[] = {
	"drop TABLE sent ;",
	"drop TABLE addressbook ;",
	"drop TABLE blacklist ;",
	"drop TABLE subscriptions ;",
	"drop TABLE whitelist ;",
	"drop TABLE inbox ;",
	"drop TABLE pubkeys ;",
	"drop TABLE inventory ;",
	"drop TABLE settings ;",
	"drop TABLE objectprocessorqueue;" ,
	/* bitmrc additions */
	"drop TABLE system;",
	"drop TABLE test_table_repl ;",
	"drop TABLE test_table_fail ;",
	"\0" };
	int sret;
	QueryResult * qr = NULL;
	StorableQueryConfig cfg(STORABLE_CREATION, NULL, 1, NULL);
	cfg.setRowSpec( RowSpec() );

	while(strlen(qt[index].query)>0) {

		cfg.setQuery(qt[index].query);
		qr = sql_exec(&cfg);
		if(qr != NULL) {
			if(qr->getError() != BITMRC_OK) {
				ret  = false;
			}
		}
		LOG_DB(("%s : exec (%s) ret : %s\n",__func__ , qt[index].query, ret==true?"true":"false"));
		index++;
	}
	last_error = ret;
	LOG_DB(("%s : ret : %d\n",__func__, ret));
	return ret;

}
/** \brief init functions for test mode
 *
 */
uint32_t Storage_mysql::init(const char * db_path) {
	LOG_DB(("%s : open db %s\n",__func__, db_name.c_str()));
	MYSQL * ldb = NULL;
	db = mysql_init(NULL);
	ldb = mysql_real_connect(db, "localhost", "bitmrc", "BitMRC", db_path, 0, "/var/run/mysqld/mysqld.sock", CLIENT_MULTI_RESULTS /* usage of sp ok */);
	LOG_DB(("  %s open db res : %p\n", __func__, ldb));
	if(ldb == db)
	{
		return BITMRC_OK;
	}
	return BITMR_DB_OPEN_FAILED;
}
#endif


/** \brief open the database with given arguments
 * for the sqlite3 type this is just the path, user information is not needed
 * set the last_error member according to the result.
 * \return true if everything is okay
 * \return false if there was an error
 */
bool Storage_mysql::open(std::string path, std::string user, std::string password) {
	db_name = path;
	db_user = user;
	db_passwd = password;
	if ((last_error = init()) == BITMRC_OK)
		return true;
	return false;
}
void Storage_mysql::flush(Storage & obj_in) {
}
bool Storage_mysql::register_storable(Storable * object) {
	// add the ref to the vector for further use. all storables using the storage must
	// have been registered.
	LOG_DB(("%s \n", __func__));
	Storage_mysql::storable_list.push_back(object);
	LOG_DB(("%s : elements in storable list now: %u\n",__func__, (unsigned int)storable_list.size()));
	// set_storables_storage(object);
	return true;
}

QueryResult * Storage_mysql::sql_exec(StorableQueryConfig * cfg)
{
	MYSQL_RES *res = NULL;
	MYSQL_ROW row = NULL;
	bool bret;
	QueryResult * ret = new QueryResult(cfg->getType());
	if (cfg == NULL) {
		ret->setError(BITMRC_DB_NO_CONFIG);
		return ret;
	}
	if(cfg->getQuery() == NULL) {
		ret->setError(BITMRC_DB_NO_QUERY);
		return ret;
	}

	// \todo: if !ret ...
	char * errinfo = NULL;
	char * query = cfg->getQuery();
	row_count = 0;
	cfg->setQueryResult(ret);
	LOG_DB(("%s: exec '%s'\n",__func__, cfg->getQuery()));
	int iret = mysql_real_query(db, query, strlen(query));

	if (iret != 0) {
		ret->setError(BITMRC_DB_EXEC_FAILED);
		ret->setNativeError(mysql_errno(db));
		ret->setErrorString(mysql_error(db));
		LOG_DB(("%s: exit. exec result %d (%s)\n",__func__, ret->getNativeError(), ret->getErrorString()));
	} else {
		res = mysql_use_result(db);
		while (res != NULL && ( prepareResultRow(res, ret) == true)) {
			row_count++;
		}
		mysql_free_result(res);
		while (mysql_next_result(db) == 0);
		ret->setError(BITMRC_OK);
	}
	char * pzTail = NULL;
	int rows = 0, sret = 0;
	return ret;
}

uint32_t Storage_mysql::get_row_count() {
	return row_count;
}
bool Storage_mysql::prepareResultRow(MYSQL_RES *res , QueryResult * qr) {
	bool bret = false;

	MYSQL_ROW row = mysql_fetch_row(res);
	if (row == NULL)
		return false;

	MYSQL_FIELD *fld  = mysql_fetch_fields(res);
	uint32_t numfields = mysql_num_fields(res);
	uint32_t fldcnt = 0;
	int32_t i32val = 0;

	QueryResultElement * el = NULL;

	while (fldcnt < numfields) {

		/** \todo: complete this, add tests */
		if( row[fldcnt] == NULL) {
			el = new QueryResultElement();
		} else if ( fld->type == MYSQL_TYPE_LONG) {
			i32val = atoi(row[fldcnt]);
			el = new QueryResultElement((int32_t)i32val);
		} else if (fld->type == MYSQL_TYPE_STRING || fld->type == MYSQL_TYPE_VAR_STRING) {
			el = new QueryResultElement(row[fldcnt], (uint32_t)strlen(row[fldcnt]));
		} else {
			el = new QueryResultElement(); /* represents a null column */
		}
#if 0
		} else if (fld->type == MYSQL_TYPE_DATE) {
		} else if (fld->type == MYSQL_TYPE_DATETIME){
		} else if ( (fld->type == MYSQL_TYPE_FLOAT || fld->type == MYSQL_TYPE_DOUBLE)) {
		} else if (fld->type == MYSQL_TYPE_TIMESTAMP) {
		} else if (config[fldcnt].type == DBU_LONGLONG && (fld->type == MYSQL_TYPE_LONGLONG)) {
		} else  {
			break;
		}
#endif
		if(el != NULL) {
			el->setName(fld->name);
			qr->addElement(el);
		}
		el = NULL;
		fld++;
		fldcnt++;
	}
	return true;
}
/** these functions have to be implemented as "callbacks" inside the storable - objects
 *
 */
/** \brief query the system table for settings
 */
uint32_t Storage_mysql::query_system_table(bitmrc_sysinfo_t * s_info) {
	uint32_t ret = BITMRC_OK;
	char * errinfo = NULL;
	if (s_info == NULL) {
		return BITMRC_BAD_PARA;
	}
	char query[] = "select node_id, working_mode, networking_flags,stream_id1, stream_id2, stream_id3, stream_id4, last_startup_timestamp, last_startup_result, database_version from system_nonexistant;";
	StorableQueryConfig cfg(query_system, query, 1, NULL);

	int mret = mysql_real_query(db, query, strlen(query));
	if (mret != 0) {
		errinfo = mysql_error(db);
		if(mysql_errno(db) == 1146)
			ret = BITMRC_DB_TABLE_NOT_EXISTANT;
		LOG_DB(("%s : Err %d (%s)\n",__func__ , mysql_errno(db), errinfo));
	}
	return ret;
}
/** \brief query the system table for settings
 */
uint32_t Storage_mysql::create_tables() {
	uint32_t ret = BITMRC_OK;
	int index = 0;
	char * errinfo = NULL;

	/* these table structures have been taken from pybitmessage 0.5.8 */
	struct querytype qt[] = {
	"CREATE TABLE addressbook (label varchar(255), address varchar(255) not null, version int, stream int, last_request DATETIME, constraint c_uq_addr UNIQUE KEY (address));",
	"CREATE TABLE blacklist (label text, address text, enabled BIT(1));",
	"CREATE TABLE inbox (msgid varchar(255), toaddress varchar(255), fromaddress varchar(255),subject varchar(255), received varchar(255), message varchar(8192), folder varchar(255), encodingtype int, was_read BIT(1), sighash varchar(255), constraint c_uq_msgid UNIQUE KEY (msgid));",
	"CREATE TABLE inventory (hash varchar(255), objecttype int, streamnumber int, payload varchar(255), expirestime integer, tag varchar(255), constraint c_uq_hash UNIQUE KEY (hash));",
	"CREATE TABLE objectprocessorqueue (objecttype int, data varchar(255), constraint c_uq_od UNIQUE KEY (objecttype, data));" ,
	"CREATE TABLE pubkeys (address varchar(255), addressversion int, signkey varchar(255), enckey varchar(255), time int, usedpersonally varchar(255), constraint c_uq_address UNIQUE KEY (address));",
	"CREATE TABLE sent (msgid blob, toaddress text, toripe blob, fromaddress text, subject text, message text, ackdata blob, senttime integer, lastactiontime integer, sleeptill integer, status text, retrynumber integer, folder text, encodingtype int, ttl int);",
	"CREATE TABLE subscriptions (label text, address text, enabled BIT(1));",
	"CREATE TABLE test_table_repl (intval int, stringval text, blobval varchar(255),constraint c_uq_intv UNIQUE KEY (intval) );",
	"CREATE TABLE test_table_fail (intval int, stringval text, blobval varchar(255),constraint c_uq_intv_2 UNIQUE _KEY (intval));",
	"CREATE TABLE whitelist (label text, address text, enabled BIT(1));",


#if 0
	mutable std::shared_timed_mutex mutex_;

	//true if not known the keys
	//false if known
	bool empty;

	int extra_bytes;
	int nonce_trials;

	int version;
	int stream;
	ustring ripe;

	time_t lastPubKeyRequest;

	ustring address;
	ustring tag;
	ustring tagE;
#endif

	"CREATE TABLE settings (key_name varchar(255), val varchar(255), constraint c_uq_key UNIQUE KEY(key_name));",
	/* bitmrc additions */
	"CREATE TABLE system (node_id int, working_mode int, networking_flags int, stream_id1 int, stream_id2 int, stream_id3 int, stream_id4 int, last_startup_timestamp int, last_startup_result int, database_version int, config_name varchar(255), constraint c_uq_cfgname UNIQUE KEY(config_name));",
	"\0" };
	int sret;
	QueryResult * qr = NULL;
	StorableQueryConfig cfg(SQC_CREATE, NULL, 1, NULL);
	cfg.setRowSpec( RowSpec() );

	while(strlen(qt[index].query)>0) {
		LOG_DB(("%s :  exec (%s)\n",__func__ , qt[index].query ));
		cfg.setQuery(qt[index].query);
		qr = sql_exec(&cfg);
		if(qr != NULL) {
			if(qr->getError() != BITMRC_OK) {
				ret = BITMRC_DB_TABLE_CREATE_FAILED;
			}
			delete qr;
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
uint32_t Storage_mysql::populate_system_table() {
	uint32_t ret = BITMRC_OK;
	int index = 0, sret;
	char * errinfo = NULL;
	/* pass the type of query plus additional data ptr to query for use in q_callback */

	struct querytype qt[] = {
	 {"call save_system_table (1000,1,1,1,0,0,0,0,1,'DEFAULT_CONFIG');"},
	 {"INSERT INTO subscriptions VALUES('Bitmessage new releases/announcements','BM-GtovgYdgs7qXPkoYaRgrLFuFKz1SFpsw',1);"},
	 {"INSERT INTO subscriptions VALUES('BitMRC Development Channel','BM-2cVxGMPpzu1WwnpUwAvcy9aQpuS9deouky',1);"},
	 {"\0"}
	};

	QueryResult * qr = NULL;
	StorableQueryConfig cfg(STORABLE_CREATION, NULL, 1, NULL);
	cfg.setRowSpec( RowSpec() );

	/* first, do completed queries */
	while(strlen(qt[index].query)>0) {
		/* keep track of last startup, so re-start loops can be avoided */
		cfg.setQuery(qt[index].query);
		qr = sql_exec(&cfg);
		if(qr != NULL) {
			if(qr->getError() != BITMRC_OK) {
				ret = BITMRC_DB_INSERT_FAILED;
			}
			delete qr;
		}
		index++;
	}
	index = 0;
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
			{{"lastvacuumtime"},{"?"}}, {{""}, {""}}
			};
	/* then: create queries for inserting settings and execute */
	while(strlen(st[index].key)>0) {
		char insert_query[256];
		sprintf(insert_query, "insert into settings (key_name,val) values ('%s','%s');",st[index].key, st[index].value);
		/* keep track of last startup, so re-start loops can be avoided */
		cfg.setQuery(insert_query);
		qr = sql_exec(&cfg);
		if(qr != NULL) {
			if(qr->getError() != BITMRC_OK) {
				ret = BITMRC_DB_INSERT_FAILED;
			}
			delete qr;
		}
		index++;
	}
	return ret;
}
