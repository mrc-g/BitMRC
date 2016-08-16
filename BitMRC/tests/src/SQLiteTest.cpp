/*
 * SQLiteTest.cpp
 *
 *  Created on: 14.08.2016
 *      Author: steady286
 */

// tests for the sqlite3 module
//
// https://github.com/steady286
#include <cpptest.h>
#include <Storage/Storage_sqlite3.h>

#include <types.h>
class SQLiteTest : public Test::Suite
{

private:
    void test_connect();
    void test_connect_nonexist();
    void test_connect_check_nonexist_systable();
    void test_create_tables();
    void populate_tables();
    // void test_connect_populate();
public:
    SQLiteTest()
    {
        TEST_ADD(SQLiteTest::test_connect)
		TEST_ADD(SQLiteTest::test_connect_nonexist)
		TEST_ADD(SQLiteTest::test_connect_check_nonexist_systable)
		TEST_ADD(SQLiteTest::test_create_tables)
		TEST_ADD(SQLiteTest::populate_tables)
    }
    ~SQLiteTest() {

    }
};

void SQLiteTest::test_connect() {
	bool ret = false;
	/** \brief init the client data module
	 * this also includes the sqlite-backend for the client
	 * \return BITMRC_OK if everything went okay and db has been opened.
	 * \return BITMRC_DB_NOT_FOUND  if db cannot be found
	 * \return BITMRC_DB_ACCESS_DENIED if access was denied
	 * \return BITMRC_CLIENT_DB_INIT_BAD if init failed
	 * \return BITMR_DB_OPEN_FAILED if open failed
	 */
	Storage_sqlite3 st;
	ret = st.open("testdb.sqlite3","","");
	TEST_ASSERT(ret == true);
	st.close();

}
void SQLiteTest::test_connect_nonexist() {
	uint32_t ret = 0;

	Storage_sqlite3 st;
	ret = st.init("/not/existing/path");
	TEST_ASSERT(ret == BITMR_DB_OPEN_FAILED);

}

void SQLiteTest::test_connect_check_nonexist_systable() {
	uint32_t ret = 0;
	bool bret = false;
	unlink("bitmrc.sqlite");
	Storage_sqlite3 st;

	bret = st.open("bitmrc.sqlite","","");
	TEST_ASSERT(bret == true);

	bitmrc_sysinfo_t sinfo;
	ret = st.query_system_table(&sinfo);
	TEST_ASSERT(ret != BITMRC_OK);
	st.close();

}
void SQLiteTest::test_create_tables() {
	uint32_t ret = 0;
	bool bret = false;
	unlink("bitmrc.sqlite");
	Storage_sqlite3 st;

	bret = st.open("bitmrc.sqlite","","");
	TEST_ASSERT(bret == true);

	bitmrc_sysinfo_t sinfo;

	ret = st.create_tables();
	TEST_ASSERT(ret == BITMRC_OK);
	ret = st.query_system_table(&sinfo);
	TEST_ASSERT(ret == BITMRC_OK);

	st.close();
}
void SQLiteTest::populate_tables() {

	uint32_t ret = 0;
	bool bret = false;
	unlink("bitmrc.sqlite");
	Storage_sqlite3 st;

	bret = st.open("bitmrc.sqlite","","");
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

}
/*
void SQLiteTest::second_test()
{
}
*/
int main(int argc, char ** argv) {
	Test::TextOutput output(Test::TextOutput::Verbose);
	SQLiteTest tst;
	return tst.run(output) ? EXIT_SUCCESS : EXIT_FAILURE;
}




