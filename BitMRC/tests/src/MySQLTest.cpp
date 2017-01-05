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
 * MySQLTest.cpp
 *
 *  Created on: 22.12.2016
 *      Author: steady286
 */

// tests for the mysql module
//
// https://github.com/steady286
#include <cpptest.h>
#include <Storage/Storage_mysql.h>

#include <types.h>
class MySQLTest : public Test::Suite
{
private:
    void test_connect();
    void test_connect_nonexist();
    void test_connect_check_nonexist_systable();
    void test_create_tables();
    void test_duplicate_data();
    void test_insert_data();
public:
    void populate_tables();
    void create_tables();
    void drop_tables();
    // void test_connect_populate();
public:
    MySQLTest()
    {
        TEST_ADD(MySQLTest::test_connect)
		TEST_ADD(MySQLTest::test_connect_nonexist)
		TEST_ADD(MySQLTest::test_connect_check_nonexist_systable)
		TEST_ADD(MySQLTest::drop_tables) /* drop first , then .. */
		TEST_ADD(MySQLTest::create_tables) /* create */
		TEST_ADD(MySQLTest::test_insert_data)
		TEST_ADD(MySQLTest::test_duplicate_data)

		//TEST_ADD(MySQLTest::test_create_tables)
		//TEST_ADD(MySQLTest::populate_tables)
    }
    ~MySQLTest() {

    }
};
#if 0
/* load settings : key|value */
void MySQLTest::test_load_settings() {
	Storage_mysql st;
	ret = st.open("bitmrc_test","bitmrc","BitMRC");
	TEST_ASSERT(ret == true);

	char query

	st.close();
}
#endif
void MySQLTest::test_connect() {
	bool ret = false;
	/** \brief init the client data module
	 * this also includes the sqlite-backend for the client
	 * \return BITMRC_OK if everything went okay and db has been opened.
	 * \return BITMRC_DB_NOT_FOUND  if db cannot be found
	 * \return BITMRC_DB_ACCESS_DENIED if access was denied
	 * \return BITMRC_CLIENT_DB_INIT_BAD if init failed
	 * \return BITMR_DB_OPEN_FAILED if open failed
	 */
	Storage_mysql st;
	ret = st.open("bitmrc_test","bitmrc","BitMRC");
	TEST_ASSERT(ret == true);
	st.close();

}
void MySQLTest::test_connect_nonexist() {
	uint32_t ret = 0;

	Storage_mysql st;
	ret = st.init("not_existing");
	TEST_ASSERT(ret == BITMR_DB_OPEN_FAILED);
}

void MySQLTest::test_connect_check_nonexist_systable() {
	uint32_t ret = 0;
	bool bret = false;

	Storage_mysql st;

	bret = st.open("bitmrc_test","bitmrc","BitMRC");
	TEST_ASSERT(bret == true);

	bitmrc_sysinfo_t sinfo;
	ret = st.query_system_table(&sinfo);
	TEST_ASSERT(ret == BITMRC_DB_TABLE_NOT_EXISTANT);
	st.close();

}
void MySQLTest::test_create_tables() {
	uint32_t ret = 0;
	bool bret = false;

	Storage_mysql st;

	bret = st.open("bitmrc_test","bitmrc","BitMRC");
	TEST_ASSERT(bret == true);

	bitmrc_sysinfo_t sinfo;

	ret = st.create_tables();
	TEST_ASSERT(ret == BITMRC_OK);
	ret = st.query_system_table(&sinfo);
	TEST_ASSERT(ret == BITMRC_OK);

	st.close();
}
void MySQLTest::create_tables() {
	Storage_mysql st;
	uint32_t result = 0;

	bool bret = st.open("bitmrc_test","bitmrc","BitMRC");
	TEST_ASSERT(bret == true);
	if (bret == true) {
		result = st.create_tables();
		TEST_ASSERT(result == BITMRC_OK)
	}
	st.close();
}
void MySQLTest::test_insert_data() {
	Storage_mysql st;
	uint32_t result = 0;

	bool bret = st.open("bitmrc_test","bitmrc","BitMRC");
	TEST_ASSERT(bret == true);
	if (bret == true) {
		result = st.populate_system_table();
		TEST_ASSERT(result == BITMRC_OK)
	}
	st.close();
}
void MySQLTest::test_duplicate_data() {
	Storage_mysql st;
	uint32_t result = 0;

	bool bret = st.open("bitmrc_test","bitmrc","BitMRC");
	TEST_ASSERT(bret == true);
	if (bret == true) {
		bret = st.drop_tables();
		TEST_ASSERT(bret == true);
		result = st.create_tables();
		TEST_ASSERT(result == BITMRC_OK)
		result = st.populate_system_table();
		TEST_ASSERT(result == BITMRC_OK)
	}
	else {
		return;
	}
	char query [] = {"call save_system_table (1000,1,1,1,0,0,0,0,1,'DEFAULT_CONFIG');"};
	char query_ins [] = {"insert into system "
			"( node_id, working_mode, networking_flags,database_version,stream_id1,stream_id2,stream_id3,stream_id4,config_name)"
			"values (1, 2, 3, 4, 5, 6 ,7, 8 ,\"DEFAULT_CONFIG\");"};

	QueryResult * qr = NULL;
	StorableQueryConfig cfg(SQC_INSERT, NULL, 1, NULL);

	cfg.setQuery(query);
	qr = st.sql_exec(&cfg);
	TEST_ASSERT(qr != NULL);
	if(qr != NULL) {
		TEST_ASSERT(qr->getError() == BITMRC_OK);
		delete qr;
	}
	/* run a query that produces a 'duplicate key' error */
	cfg.setQuery(query_ins);
	qr = st.sql_exec(&cfg);
	TEST_ASSERT(qr != NULL);
	if(qr != NULL) {
		TEST_ASSERT(qr->getError() == BITMRC_DB_EXEC_FAILED);
		TEST_ASSERT(qr->getNativeError() == 1062);
		delete qr;
	}
	st.close();
}
void MySQLTest::drop_tables() {
	Storage_mysql st;

	bool bret = st.open("bitmrc_test","bitmrc","BitMRC");
	TEST_ASSERT(bret == true);
	if (bret == true) {
		bret = st.drop_tables();
		TEST_ASSERT(bret == true);
	}
	st.close();
}
void MySQLTest::populate_tables() {

	uint32_t ret = 0;
	bool bret = false;

	Storage_mysql st;

	bret = st.open("bitmrc_test","bitmrc","BitMRC");
	TEST_ASSERT(bret == true);

	bitmrc_sysinfo_t sinfo;

	ret = st.create_tables();
	TEST_ASSERT(ret == BITMRC_OK);

	ret = st.populate_system_table();
	TEST_ASSERT(ret == BITMRC_OK);

	ret = st.query_system_table(&sinfo);
	TEST_ASSERT(ret == BITMRC_OK);

	TEST_ASSERT(sinfo.database_version == 1);
	TEST_ASSERT(sinfo.node_id == 1000);
	st.close();
}

int main(int argc, char ** argv) {
	Test::TextOutput output(Test::TextOutput::Verbose);
	MySQLTest tst;
	return tst.run(output) ? EXIT_SUCCESS : EXIT_FAILURE;
}




