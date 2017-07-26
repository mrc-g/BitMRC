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
 * StorableAddressTest.cpp
 *
 *  Created on: 09.06.2016
 *      Author: steady286
 *      tests for 'configuration' storable load/save etc.
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
#include <Configuration.h>
#include <StorableAddressTest.h>
#include <Storage/Storable.h>
#include <Storage/Storage.h>
#include <Storage/StorableQueryConfig.h>
#include <Storage/Storage_sqlite3.h>
#include <Storage/Storage_mysql.h>
#include <Storage/RowSpec.h>
#include <sys/time.h>
#include <sha3.h>
#include <utils.h>
#include <Addr.h>

Addr ad;

void StorableAddressTest::create_addr() {
	bool bret = ad.generateRandom();
	TEST_ASSERT(bret == true);
	int i = 0, nonce = 0;

	ustring priv_addr = ad.buildAddressFromKeys(ad.getPrivSigningKey(), ad.getPrivEncryptionKey(), ad.getStream(), ad.getVersion());
	ustring pub_addr = ad.buildAddressFromKeys(ad.getPubSigningKey(), ad.getPubEncryptionKey(), ad.getStream(), ad.getVersion());

	ustring pe = ad.getPrivEncryptionKey();
	ustring ps = ad.getPrivSigningKey();

	cout << "PrivAddr " << priv_addr.c_str() << " PubAddr " << pub_addr.c_str() << endl;
	cout << "PrivEnc Key" << endl << pe.toHexString().c_str() << endl;
	cout << "PrivSign Key" << endl << ps.toHexString().c_str() << endl;

	cout << "PubEnc Key" << endl << ad.getPubOfPriv(ad.getPrivEncryptionKey()).toHexString().c_str() << endl;
	cout << "PubSign Key" << endl << ad.getPubOfPriv(ad.getPrivSigningKey()).toHexString().c_str() << endl;

	Addr pa;
	//
	string pass("Bitmessage new releases/announcements");

	ustring pass_phrase, priv_addr_2;
	pass_phrase.clear();
	priv_addr_2.clear();
	pass_phrase.fromString(pass);
	/*Bitmessage new releases/announcements BM-2cTW5AYhGbFnCBnUG9FVYz42bYDN9PW9XD DetPubAddr BM-87TUUQRa8Sdiifkj1LnNRKb1eadu9ixP6aM */
	for(;i<2;i++) {
		nonce = pa.generateDeterministic(pass_phrase, nonce);
		priv_addr_2 = pa.buildAddressFromKeys(pa.getPubSigningKey(), pa.getPubEncryptionKey(), pa.getStream(), pa.getVersion());
		cout << "[" << nonce << "|" << pass_phrase.c_str() <<"] DetPubAddr " << priv_addr_2.c_str();
		//cout << "  Hex " << endl << priv_addr_2.toHexString().c_str() << endl;
		priv_addr_2 = pa.buildAddressFromKeys(pa.getPrivSigningKey(), pa.getPrivEncryptionKey(), pa.getStream(), pa.getVersion());
		cout << " DetPrivAddr " << priv_addr_2.c_str() << endl;
	}

	pass = "BitMRC Development Channel";

	pass_phrase.clear();
	priv_addr_2.clear();
	pass_phrase.fromString(pass);
	/*Bitmessage new releases/announcements BM-2cTW5AYhGbFnCBnUG9FVYz42bYDN9PW9XD DetPubAddr BM-87TUUQRa8Sdiifkj1LnNRKb1eadu9ixP6aM */
	for(i=0;i<2;i++) {
		nonce = pa.generateDeterministic(pass_phrase, nonce);
		priv_addr_2 = pa.buildAddressFromKeys(pa.getPubSigningKey(), pa.getPubEncryptionKey(), pa.getStream(), pa.getVersion());
		cout << "[" << nonce << "|" << pass_phrase.c_str() <<"] DetPubAddr " << priv_addr_2.c_str();
		//cout << "  Hex " << endl << priv_addr_2.toHexString().c_str() << endl;
		priv_addr_2 = pa.buildAddressFromKeys(pa.getPrivSigningKey(), pa.getPrivEncryptionKey(), pa.getStream(), pa.getVersion());
		cout << " DetPrivAddr " << priv_addr_2.c_str() << endl;
	}
}

void StorableAddressTest::init_db_mysql() {

	bool bret = false;
	uint32_t ret = BITMRC_OK;
	/* sub-class to all known types. wrong one will get NULL*/
	strg_mysql = new Storage_mysql();
	TEST_ASSERT(strg_mysql != NULL);
	if(strg_mysql == NULL)
		return;
	strg = strg_mysql;

	strg_mysql->open("bitmrc_test","bitmrc" /*user*/ ,"BitMRC" /*pwd*/);

	bret = strg_mysql->drop_tables();
	TEST_ASSERT(bret == true);
	ret = strg_mysql->create_tables();
	TEST_ASSERT(ret == BITMRC_OK);
	if (ret == BITMRC_OK) {

		TEST_ASSERT(ret == BITMRC_OK);
		ret = strg_mysql->populate_system_table();
		TEST_ASSERT(ret == BITMRC_OK);
	}

}
StorableAddressTest::StorableAddressTest() : strg(NULL), strg_mysql(NULL) {
	TEST_ADD(StorableAddressTest::create_addr);

	TEST_ADD(StorableAddressTest::init_db_mysql);
	TEST_ADD(StorableAddressTest::test_02_store_addr);
	TEST_ADD(StorableAddressTest::test_04_store_pub_addr);
	/*
	TEST_ADD(StorableAddressTest::test_01_load_addr);
	TEST_ADD(StorableAddressTest::test_03_load_pub_addr);
	TEST_ADD(StorableAddressTest::close_test_mysql);
	*/
}

StorableAddressTest::~StorableAddressTest() {
	if(strg_mysql)
		strg_mysql->close();
	delete strg_mysql;
}
void StorableAddressTest::test_01_load_addr() {
	bool bret = false;

	PubAddr pa;
	Addr ad;
	int rowcount = 0;

	bret = ad.generateRandom();
	TEST_ASSERT(bret == true);

	ustring pe = ad.getPrivEncryptionKey();
	ustring ps = ad.getPrivSigningKey();

	ustring priv_addr = ad.buildAddressFromKeys(ad.getPrivSigningKey(), ad.getPrivEncryptionKey(), ad.getStream(), ad.getVersion());

	cout << "Address : " << priv_addr.c_str() << endl;

	QueryResult * qr = NULL;
	Unique_Key uq = pa.calc_key();
	pa.set_key(uq);

	StorableQueryConfig cfg(SQC_SELECT, ad.get_load_query(),1, NULL);
	qr = strg_mysql->sql_exec(&cfg);
	TEST_ASSERT(qr->getError() == BITMRC_OK);
}
void StorableAddressTest::test_02_store_addr() {

	PubAddr pa;
	Addr ad;
	int i;
	int rowcount = 0;
	for(i=0;i<10;i++) {
		bool bret = ad.generateRandom();
		TEST_ASSERT(bret == true);

		ustring pub_addr = ad.buildAddressFromKeys(ad.getPubSigningKey(), ad.getPubEncryptionKey(), ad.getStream(), ad.getVersion());

		cout << "Address : " << pub_addr.c_str() << endl;

		QueryResult * qr = NULL;
		Unique_Key uq = pa.calc_key();
		pa.set_key(uq);

		StorableQueryConfig cfg(SQC_INSERT, ad.get_update_query(),1, NULL);
		qr = strg_mysql->sql_exec(&cfg);
		TEST_ASSERT(qr->getError() == BITMRC_OK);
	}

}

void StorableAddressTest::test_04_store_pub_addr() {

	PubAddr pub_a;
	Addr ad;
	bool bret;
	int i;
	int rowcount = 0;
	for(i=0;i<10;i++) {

		bret = ad.generateRandom();
		TEST_ASSERT(bret == true);

		pub_a.loadKeys(ad.getPubOfPriv(ad.getPrivSigningKey()), ad.getPubOfPriv(ad.getPrivEncryptionKey()), ad.getNonce(), ad.getExtra());

		ustring pub_addr = pub_a.buildAddressFromKeys(	ad.getPubOfPriv(ad.getPrivSigningKey()),
													ad.getPubOfPriv(ad.getPrivEncryptionKey()),
													ad.getStream(), ad.getVersion());

		ustring pa2 = pub_a.buildAddressFromKeys(pub_a.getPubSigningKey(), pub_a.getPubEncryptionKey(), pub_a.getStream(), pub_a.getVersion());

		// cout << "++ Address : " << pub_addr.c_str() << " Address2 : " << pa2.c_str() << endl;
		TEST_ASSERT(pa2 == pub_addr);

		QueryResult * qr = NULL;
		Unique_Key uq = pub_a.calc_key();
		pub_a.set_key(uq);
		/*char * q = pa.get_update_query();
		cout << "+++ len(query) " << strlen(q) << endl;*/
		StorableQueryConfig cfg(SQC_INSERT, pub_a.get_update_query(),1, NULL);
		qr = strg_mysql->sql_exec(&cfg);
		TEST_ASSERT(qr->getError() == BITMRC_OK);
	}

}
void StorableAddressTest::test_03_load_pub_addr() {
	TEST_ASSERT(1==0);
}

void StorableAddressTest::close_test_mysql() {
	TEST_ASSERT(1==0);
}
int main(int argc, char ** argv) {
	bool result ;
	Test::TextOutput output(Test::TextOutput::Verbose);
	StorableAddressTest ts;
	result = ts.run(output);
	return result ? EXIT_SUCCESS : EXIT_FAILURE;
}

