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
 * StorableConfigrationTest.cpp
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
#include <Storage/StorableConfiguration.h>
#include <StorableConfigurationTest.h>
#include <Storage/Storable.h>
#include <Storage/Storage.h>
#include <Storage/StorableQueryConfig.h>
#include <Storage/Storage_sqlite3.h>
#include <Storage/Storage_mysql.h>
#include <Storage/RowSpec.h>
#include <Configuration.h>
#include <sys/time.h>
#include <sha3.h>
#include <utils.h>
#include <Addr.h>




Unique_Key StorableConfigurationTest::calc_key() {
	static char digest[] = {"DEFAULT_CONFIG"};
	Unique_Key uq;
	uq.set_digest(digest);
	return uq;
}
void StorableConfigurationTest::init() {
	/* sub-class to all known types. wrong one will get NULL
	strg_sqlite3 = dynamic_cast<Storage_sqlite3*>(this->strg);
	strg_mysql = dynamic_cast<Storage_mysql*>(this->strg);*/

}
StorableConfigurationTest::StorableConfigurationTest() : strg(NULL){
	s_cfg = new StorableConfiguration();


	TEST_ADD(StorableConfigurationTest::init_test_mysql);
	TEST_ADD(StorableConfigurationTest::init_db_mysql);
	TEST_ADD(StorableConfigurationTest::test_01_load_config_mysql);
	TEST_ADD(StorableConfigurationTest::test_03_query_result_check_mysql);
	TEST_ADD(StorableConfigurationTest::test_02_new_config_mysql);
	TEST_ADD(StorableConfigurationTest::test_05_find_by_key);
	TEST_ADD(StorableConfigurationTest::close_test_mysql);
}

StorableConfigurationTest::~StorableConfigurationTest() {
	strg->close();
	if(s_cfg!=NULL)
		delete s_cfg;
	delete strg;
}

bool StorableConfigurationTest::query(Unique_Key &uq_key_in, string & data_out) {
	return true;
}

char * StorableConfigurationTest::get_update_query() {
	return s_cfg->get_update_query();
}

char * StorableConfigurationTest::get_load_query() {
	return s_cfg->get_load_query();
}
bool StorableConfigurationTest::store() {
	Unique_Key out = calc_key();
	char * query = this->get_update_query();
	QueryResult * qr = NULL;
	uint32_t ui_out ;
	bool bret = false;

	StorableQueryConfig cfg(SQC_UPDATE, get_update_query(), 1, NULL);
	strg->sql_exec(&cfg);
	if ( cfg.getBitMRCError() == BITMRC_OK) {
		qr = cfg.getQueryResult();
		if (qr) {
			bret = qr->getFieldUint32("database_version", &ui_out);
			TEST_ASSERT(bret == true);
		}
	}
	return true;
}
bool store(Storable & object_in, Unique_Key & key_out) {
	return true;
}

bool StorableConfigurationTest::delete_storable(Storable & object_in) {
	return true;
}

bool StorableConfigurationTest::delete_storable(Unique_Key & key_in) {
	return true;
}

bool StorableConfigurationTest::find_by_key(Unique_Key & key_in) {

	char query[256] ;
	bool ret = false;
	uint32_t out_val = 0;

	sprintf(query, "select node_id, working_mode, networking_flags, stream_id1, stream_id2, stream_id3, stream_id4, "
			"last_startup_timestamp, last_startup_result, database_version from system where config_name = \"%s\"" , key_in.get_key().c_str());

	QueryResult * qr = NULL;
	StorableQueryConfig q_cfg(SQC_SELECT, query, 1, NULL);
	q_cfg.setQuery(query);

	qr = strg->sql_exec(&q_cfg);
//	if (strg->get_row_count()>0) {
		ret = true;
		this->s_cfg->set_invalid();
		ret &= qr->getFieldUint32("node_id", &out_val);
		if(ret == true) this->s_cfg->set_node_id(out_val);
		ret &= qr->getFieldUint32("working_mode", &out_val) ;
		if(ret == true) this->s_cfg->set_working_mode(out_val);
		ret &= qr->getFieldUint32("networking_flags", &out_val) ;
		if(ret == true) this->s_cfg->set_networking_flags(out_val);
		ret &= qr->getFieldUint32("stream_id1", &out_val) ;
		if(ret == true) this->s_cfg->set_stream_id1(out_val);
		ret &= qr->getFieldUint32("stream_id2", &out_val) ;
		if(ret == true) this->s_cfg->set_stream_id2(out_val);
		ret &= qr->getFieldUint32("stream_id3", &out_val) ;
		if(ret == true) this->s_cfg->set_stream_id3(out_val);
		ret &= qr->getFieldUint32("stream_id4", &out_val) ;
		if(ret == true) this->s_cfg->set_stream_id4(out_val);
		ret &= qr->getFieldUint32("last_startup_timestamp", &out_val) ;
		if(ret == true) this->s_cfg->set_last_startup_time(out_val);
		ret &= qr->getFieldUint32("last_startup_result", &out_val) ;
		if(ret == true) this->s_cfg->set_last_startup_result(out_val);
		ret &= qr->getFieldUint32("database_version", &out_val) ;
		if(ret == true) this->s_cfg->set_database_version(out_val);
		if(ret == true) this->s_cfg->set_valid();
//	}
	return ret;
}
// Tests ------------------------------------
void StorableConfigurationTest::storeMe() {
	Unique_Key uq;
	store();
}
void  StorableConfigurationTest::init_test_mysql() {
	uint32_t ret = 12;
	bool bret = false;
	Storage_mysql * strg_mysql = new Storage_mysql();
	TEST_ASSERT(strg_mysql != NULL);
	if(strg_mysql!=NULL) {
		strg = strg_mysql;
		s_cfg->set_storage(strg_mysql);
	}
}
void StorableConfigurationTest::init_db_mysql() {
	uint32_t ret = 12;
	bool bret = false;
	Storage_mysql * strg_mysql = dynamic_cast<Storage_mysql*>(strg);
	TEST_ASSERT(strg_mysql != NULL);
	if(strg_mysql!=NULL) {
		strg_mysql->open("bitmrc_test","bitmrc" /*user*/ ,"BitMRC" /*pwd*/);
		strg = strg_mysql;
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
}
void StorableConfigurationTest::CalcKey() {
	Unique_Key uqk = this->calc_key();
	if(uqk.get_digest_len() != 16) {
		cout << "digest len " << uqk.get_digest_len() << endl;
	}
	TEST_ASSERT(uqk.get_digest_len() == 16); /* ?? */
}
void StorableConfigurationTest::close_test_mysql() {
	strg->close();
	TEST_ASSERT(0==0);


}
// Tests ------------------------------------

void StorableConfigurationTest::test_01_load_config_mysql () {

	static char query [] = "select node_id,working_mode,networking_flags,stream_id1,stream_id2,stream_id3,stream_id4,"
			"last_startup_timestamp,last_startup_result,database_version from system where config_name = \"DEFAULT_CONFIG\";";

	Storable * val = NULL;

	StorableQueryConfig cfg(SQC_SELECT, query, 1, NULL);

	QueryResult * rslt = strg->sql_exec(&cfg);
	TEST_ASSERT(rslt->getNumColumns() == 10);

	delete rslt;

}
void StorableConfigurationTest::test_02_new_config_mysql() {

	uint32_t ui_out = 0;
	bool bret = false;
	Unique_Key k;
	k.set_digest("DEFAULT_CONFIG");
	QueryResult*  qr=NULL;
	Storage_mysql * strg_mysql = dynamic_cast<Storage_mysql*>(strg);
	StorableQueryConfig q_cfg(SQC_UPDATE, s_cfg->get_update_query(), 1, NULL);
	strg_mysql->sql_exec(&q_cfg);
	if ( q_cfg.getBitMRCError() == BITMRC_OK) {
		qr = q_cfg.getQueryResult();
		if (qr) {
			TEST_ASSERT(qr->getNumColumns() == 0);
		}
	}
	delete qr;
}
void StorableConfigurationTest::test_03_query_result_check_mysql () {

//	char query [] = "select node_id, working_mode, networking_flags, stream_id1, stream_id2, stream_id3, stream_id4, "
//			"last_startup_timestamp, last_startup_result, database_version from system where config_name = \"DEFAULT_CONFIG\";";

	Storable * val = NULL;

	QueryResult qr(QR_TYPE_NONE);
	StorableQueryConfig cfg(SQC_SELECT, s_cfg->get_load_query(), 1, &qr);

	uint32_t out1;
	int32_t out2;
	uint64_t out3;
	int64_t out4;
	Storage_mysql * strg_mysql = dynamic_cast<Storage_mysql*>(strg);
	TEST_ASSERT(strg_mysql != NULL);
	if (strg_mysql == NULL)
		return;

	QueryResult * rslt = strg_mysql->sql_exec(&cfg);
	TEST_ASSERT(rslt->getFieldUint32("node_id", &out1) == true);
	TEST_ASSERT(out1 == 1000);
	TEST_ASSERT(rslt->getFieldUint32("working_mode",&out1) == true);
	TEST_ASSERT(out1 == 1);
	TEST_ASSERT(rslt->getFieldUint32("networking_flags",&out1) == true);
	TEST_ASSERT(out1 == 1);
	TEST_ASSERT(rslt->getFieldUint32("stream_id1",&out1) == true);
	TEST_ASSERT(out1 == 1);
	TEST_ASSERT(rslt->getFieldUint32("stream_id2",&out1) == true);
	TEST_ASSERT(out1 == 0);
	TEST_ASSERT(rslt->getFieldUint32("stream_id3",&out1) == true);
	TEST_ASSERT(out1 == 0);
	TEST_ASSERT(rslt->getFieldUint32("stream_id4",&out1) == true);
	TEST_ASSERT(out1 == 0);

	TEST_ASSERT(rslt->getFieldUint32("database_version",&out1) == true);
	TEST_ASSERT(out1 == 1);
	delete rslt;

}
void StorableConfigurationTest::test_05_find_by_key () {
	Storable * val = NULL;
	Unique_Key uk;
	uk.set_key("DEFAULT_CONFIG");
	bool bval = find_by_key(uk);
	TEST_ASSERT(bval == true);
}

int main(int argc, char ** argv) {
	bool result ;
	Test::TextOutput output(Test::TextOutput::Verbose);
	StorableConfigurationTest ts;
	result = ts.run(output);
	return result ? EXIT_SUCCESS : EXIT_FAILURE;
}

