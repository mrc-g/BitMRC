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
 * StorableConfigurationTest.h
 *
 *  Created on: 31.12.2016
 *      Author: steady286
 */

#ifndef STORABLECONFIGURATIONTEST_H_
#define STORABLECONFIGURATIONTEST_H_
using namespace std;
class StorableConfigurationTest : public Test::Suite
{
public:
	StorableConfigurationTest();
	StorableConfigurationTest(Storage *);
	~StorableConfigurationTest();
public:
	bool query(Unique_Key &uq_key_in, string & data_out);
	bool store();
	bool load(Storable & object_in, Unique_Key & key_out);
	bool delete_storable(Storable & object_in);
	bool delete_storable(Unique_Key & key_in);
	bool find_by_key(Unique_Key & key_in);
public:
	void CalcKey();
	void opendb();

	void storeMe();

public:
	char * get_update_query();
	char * get_load_query();

	void init_test_mysql();
	void init_db_mysql();
	void test_00_drop_tables_mysql();
	void test_01_load_config_mysql();
	void test_02_new_config_mysql();
	void test_03_query_result_check_mysql();
	void test_05_find_by_key();
	void close_test_mysql();

	void TestAddr();
	void ConfigTest();

protected:
    Unique_Key calc_key();
    void print();
    void init();

protected:
    StorableConfiguration * s_cfg;
    Storage * strg;

};



#endif /* STORABLECONFIGURATIONTEST_H_ */
