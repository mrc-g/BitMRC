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
#include <sys/time.h>
#include <sha3.h>

#ifdef DEBUG_STORABLES
#define STORABLE_DEBUG(a) printf a;
#else
#define STORABLE_DEBUG(a)
#endif

class StorableTest : public Storable, public Test::Suite
{
public:
	Unique_Key calc_key();
	bool query(Unique_Key &uq_key_in, string & data_out);
	bool store(Storable & object_in, Unique_Key & key_out);
	bool delete_storable(Storable & object_in);
	bool delete_storable(Unique_Key & key_in);
	Storable & 	find_by_key(Unique_Key & key_in);
public:
	void init_test();
	void TestA();
public:
	StorableTest();
    ~StorableTest();
};
void StorableTest::init_test() {
    TEST_ASSERT(1 == 1);
}
void StorableTest::TestA() {
	StorableTest strbl;
	Unique_Key uq = strbl.calc_key();
	TEST_ASSERT(1 == 1);
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
	generation_time = 0;
	TEST_ADD(StorableTest::TestA)
	//TEST_ADD(StorableTest::TestB)
    // TEST_ADD(EndianTest::second_test)
}
StorableTest::~StorableTest() {};

Unique_Key StorableTest::calc_key() {
	Unique_Key uq;
	unsigned char key_string[120];
	// SHA3_256 sha;

	std::string str;
	if(generation_time == 0) {
		/* if no gentime is set, do not use default value for generation as this would
		 * create only one value */
		struct timeval tv = {.0};
		gettimeofday(&tv, NULL);
		this->generation_time = tv.tv_sec * 1000 * 1000 + tv.tv_usec;
	}
	sprintf(key_string, "%05hu%llu", this->type, this->generation_time);
	str.assign(key_string);
	uq.set_key(str);

	STORABLE_DEBUG(("KEY is %s\n", key_string));
	return uq;
}
bool StorableTest::query(Unique_Key &uq_key_in, string & data_out) {
	return true;
}
bool StorableTest::store(Storable & object_in, Unique_Key & key_out) {
	return true;
}
bool StorableTest::delete_storable(Storable & object_in) {
	return true;
}
bool StorableTest::delete_storable(Unique_Key & key_in) {
	return true;
}
Storable & StorableTest::find_by_key(Unique_Key & key_in) {
	StorableTest val;
	return val;
}
void StorableTest::init_test();
void StorableTest::TestA();




int main(int argc, char ** argv) {
	Test::TextOutput output(Test::TextOutput::Verbose);
	StorableTest ts;
	return ts.run(output) ? EXIT_SUCCESS : EXIT_FAILURE;
}




