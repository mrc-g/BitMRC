/*
 * Storage_sqlite3.cpp
 *
 *  Created on: 28.05.2016
 *      Author: steady286
 *
 *      Storage backend for sqlite3 flat file db
 *
 */
#include <StorageInterface.h>
#include <Storage_sqlite3.h>
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
#include <StorageInterface.h>
#include <Storage_sqlite3.h>
#include <vector>
#include <iostream>
vector<Storable *> Storage::storable_list;
//vector<Storable *> Storage_sqlite3::storable_list ;

Storage_sqlite3::Storage_sqlite3() {
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

