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
#include <utils.h>
#include <Addr.h>


class StorableTest : public Storable, public Test::Suite
{
public:
	//Unique_Key calc_key();
	bool query(Unique_Key &uq_key_in, string & data_out);
	bool store(Storable & object_in, Unique_Key & key_out);
	bool delete_storable(Storable & object_in);
	bool delete_storable(Unique_Key & key_in);
	Storable & 	find_by_key(Unique_Key & key_in);
public:
	void init_test();
	void TestA();
	void TestAddr();
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
	generation_time = 0;
	TEST_ADD(StorableTest::TestA)
	TEST_ADD(StorableTest::TestAddr)
    // TEST_ADD(EndianTest::second_test)
}
StorableTest::~StorableTest() {};
#if 0
Unique_Key StorableTest::calc_key() {
	Unique_Key uq;
	unsigned char key_string[120];
	// SHA3_256 sha;

	std::string tmp_str;
	ustring str;

	tmp_str.clear();
	str.clear();
	if(generation_time == 0) {
		/* if no gentime is set, do not use default value for generation as this would
		 * create only one value */
		struct timeval tv = {.0};
		gettimeofday(&tv, NULL);
		this->generation_time = tv.tv_sec * 1000 * 1000 + tv.tv_usec;
	}
	/* key format : type and time so we can search for specific objects generated at certain times */
	sprintf(key_string, "%05hu%llu", this->type, this->generation_time);
	tmp_str.assign(key_string);
	str.assign(key_string);
	uq.set_key(tmp_str);
#ifdef DEBUG_STORABLES
	std::string hex_str;
	hex_str.clear();
	hex_str = str.toHexString();
	STORABLE_DEBUG(("%s %s KEY is (%s) \n", __FILE__, __func__, key_string));
	STORABLE_DEBUG(("%s %s HEX is \n%s\n", __FILE__, __func__, hex_str.c_str()));
#endif
	return uq;
}
#endif
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




