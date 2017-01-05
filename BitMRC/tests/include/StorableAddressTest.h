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
 * StorableAddressTest.h
 *
 *  Created on: 29.12.2016
 *      Author: steady286
 */

#ifndef STORABLE_ADDRESSTEST_H_
#define STORABLE_ADDRESSTEST_H_
#include <Addr.h>
#include <Configuration.h>
#include <Storage/Storable.h>
#include <Storage/Storage_sqlite3.h>
#include <Storage/Storage_mysql.h>

class StorableAddressTest : public Test::Suite
{
public:
	StorableAddressTest();
	StorableAddressTest(Storage *);
	~StorableAddressTest();
public:
	void init_test_mysql();
	void create_addr();
	void init_db_mysql();
	void test_01_load_addr();
	void test_02_store_addr();
	void test_03_load_pub_addr();
	void test_04_store_pub_addr();
	void close_test_mysql();
protected:
    Storage * strg;
    Storage_mysql * strg_mysql;
};

#endif
