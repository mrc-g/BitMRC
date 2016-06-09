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
/*
 * this has been taken from pybitmessage 0.5.8
CREATE TABLE inbox (msgid blob, toaddress text, fromaddress text, subject text, received text, message text, folder text, encodingtype int, read bool, sighash blob, UNIQUE(msgid) ON CONFLICT REPLACE)''' )
CREATE TABLE sent (msgid blob, toaddress text, toripe blob, fromaddress text, subject text, message text, ackdata blob, senttime integer, lastactiontime integer, sleeptill integer, status text, retrynumber integer, folder text, encodingtype int, ttl int)''' )
CREATE TABLE subscriptions (label text, address text, enabled bool)''' )
CREATE TABLE addressbook (label text, address text)''' )
CREATE TABLE blacklist (label text, address text, enabled bool)''' )
CREATE TABLE whitelist (label text, address text, enabled bool)''' )
CREATE TABLE pubkeys (address text, addressversion int, transmitdata blob, time int, usedpersonally text, UNIQUE(address) ON CONFLICT REPLACE)''' )
CREATE TABLE inventory (hash blob, objecttype int, streamnumber int, payload blob, expirestime integer, tag blob, UNIQUE(hash) ON CONFLICT REPLACE)''' )
INSERT INTO subscriptions VALUES('Bitmessage new releases/announcements','BM-GtovgYdgs7qXPkoYaRgrLFuFKz1SFpsw',1)''')
INSERT INTO subscriptions VALUES('BitMRC Development Channel','BM-2cVxGMPpzu1WwnpUwAvcy9aQpuS9deouky',1)''')

CREATE TABLE settings (key blob, value blob, UNIQUE(key) ON CONFLICT REPLACE)''' )
INSERT INTO settings VALUES('version','10')''')
INSERT INTO settings VALUES('lastvacuumtime',?)''', (
CREATE TABLE objectprocessorqueue (objecttype int, data blob, UNIQUE(objecttype, data) ON CONFLICT REPLACE)''' )
*/
#include <vector>
#include <iostream>
#include <mutex>
#include <stdint.h>
#include <sqlite3.h> /** need to use: packages libsqlite3-dev and sqlite3 on linux */

vector<Storable *> Storage::storable_list;

/** \brief helper for determining if passed int value is an sqlite3 error code
 *
 */
static int Storage_sqlite3::is_sqlite_error(int errin) {

	return 0;
}
/** \brief the needed callback function
 *
 */
static int Storage_sqlite3::q_callback(void* param ,int count, char** keys, char** values) {

	return 0;
}
void Storage_sqlite3::close() {
	if (db!=NULL)
		sqlite3_close(db);
}

/** \brief init the client data module
 * this also includes the sqlite-backend for the client
 *
 */
uint32_t Storage_sqlite3::init() {
	db = NULL;
	return 0;
}
/** \brief
 */
Storage_sqlite3::Storage_sqlite3() {
	init();
	storable_list.clear();
}
Storage_sqlite3::~Storage_sqlite3() {
	cout << "Storage_sqlite3 destructor" << endl;
}

// methods needed by all Storage objects
bool Storage_sqlite3::open(std::string path, std::string user, std::string password) {

	return true;
}
bool Storage_sqlite3::close(Storage & obj_in) {
	return true;
}
void Storage_sqlite3::flush(Storage & obj_in) {

}
bool Storage_sqlite3::register_storable(Storable * object) {
	// add the ref to the vector for further use. all storables using the storage must
	// have been registered.
	cout << "Storage_sqlite3 %s" << __func__ << endl;

	return true;
}

