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
 * StorageTest.cpp
 *
 *  Created on: 09.06.2016
 *      Author: steady286
 *      tests for the storable/storage mechanism
 */
#include <cpptest.h>
#include <Storage/Storable.h>
#include <Storage/Storage.h>
#include <Storage/Storage_sqlite3.h>
	#include <Configuration.h>
#include <sys/time.h>
#include <sha3.h>
#include <utils.h>
#include <Addr.h>

#define TEST_DB_NAME "storabletest.sqlite3"
#define TEST_DB2_NAME "storabletest2.sqlite3"

Storage_sqlite3 st;

class StorableTest : public Storable, public Test::Suite
{
public:
	//Unique_Key calc_key();
	bool query(Unique_Key &uq_key_in, string & data_out);
	bool store();
	bool delete_storable(Storable & object_in);
	bool delete_storable(Unique_Key & key_in);
	bool find_by_key(Unique_Key & key_in);
public:
	void init_test();
	void CalcKey();
	void TestAddr();
	void ConfigTest();
	void opendb();
	void closedb();
	void storeMe();
public:
	StorableTest();
    ~StorableTest();
public:
    Unique_Key calc_key();
};
void StorableTest::init_test() {
	//unlink(TEST_DB_NAME);
	//unlink(TEST_DB2_NAME);
    TEST_ASSERT(1 == 1);
}

void StorableTest::CalcKey() {
	Unique_Key uqk = this->calc_key();
	if(uqk.get_digest_len() != 16) {
		cout << "digest len " << uqk.get_digest_len() << endl;
	}
	TEST_ASSERT(uqk.get_digest_len() == 16); /* ?? */
}

void StorableTest::opendb() {
	bool openok = st.open(TEST_DB_NAME,"user","password") ;
	TEST_ASSERT(openok == true);
}
void StorableTest::closedb() {
	st.close();
	TEST_ASSERT(0==0);
}
void StorableTest::ConfigTest() {
	bool result;
	Configuration cfg;
	Storage_sqlite3 storage;
	result = storage.open(TEST_DB2_NAME,"user","password") ;
	TEST_ASSERT(result == true);
	if(result) {
		//storage.register_storable(&cfg);
		//result = cfg.load();
	}

}
Unique_Key StorableTest::calc_key() {

	Unique_Key uq;
#ifdef DEBUG_STORABLES
	cout << "time: " << this->generation_time << " type " << this->type << endl << "Generated Key: "<< uq.get_key() << "Digest (global object id) is: "<< uq.digest_to_string() << endl;
#endif
	TEST_ASSERT(1 == 1);
	return uq;
}


void StorableTest::TestAddr() {

	PubAddr pa;
	Unique_Key uk = pa.calc_key();

}
#if 0
void StorableTest::TestB() {
	TEST_ASSERT(1 == 1);
}
#endif

#if 0
void ExampleTestSuite::second_test()
{
}
#endif
StorableTest::StorableTest() {
	type = STORABLE_TEST;
	//generation_time = 0;
	TEST_ADD(StorableTest::CalcKey)
	TEST_ADD(StorableTest::TestAddr)
	TEST_ADD(StorableTest::ConfigTest)
    TEST_ADD(StorableTest::opendb)
    TEST_ADD(StorableTest::storeMe)
    TEST_ADD(StorableTest::closedb)
}
StorableTest::~StorableTest() {
	st.close();
	// unlink(TEST_DB_NAME);
	// unlink(TEST_DB2_NAME);

}
bool StorableTest::query(Unique_Key &uq_key_in, string & data_out) {
#if 0
	int sqlite3_prepare_v2(
	  sqlite3 *db,            /* Database handle */
	  const char *zSql,       /* SQL statement, UTF-8 encoded */
	  int nByte,              /* Maximum length of zSql in bytes. */
	  sqlite3_stmt **ppStmt,  /* OUT: Statement handle */
	  const char **pzTail     /* OUT: Pointer to unused portion of zSql */
	);
#endif

	return true;
}
bool StorableTest::store() {
	Unique_Key out = calc_key();

	return true;
}

bool StorableTest::delete_storable(Storable & object_in) {
	return true;
}
bool StorableTest::delete_storable(Unique_Key & key_in) {
	return true;
}

bool StorableTest::find_by_key(Unique_Key & key_in) {
	//StorableTest * val = new StorableTest();
	return false	;
}

void StorableTest::storeMe() {
	Unique_Key uq;
	store();
	uq = this->calc_key();
#ifdef DEBUG_STORABLES
	cout << "Stored... UQK is " << uq.get_key() << endl;

#endif
}


int main(int argc, char ** argv) {
	bool result ;
	Test::TextOutput output(Test::TextOutput::Verbose);
	StorableTest ts;
	result = ts.run(output);



	return result ? EXIT_SUCCESS : EXIT_FAILURE;
}




