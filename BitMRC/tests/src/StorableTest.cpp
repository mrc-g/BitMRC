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

class StorableTest : public Storable, public Test::Suite
{
public:
	Unique_Key calc_key(Storable & object_in) {
		Addr addr;
		Unique_Key uq;
		addr.generateRandom();
		uq.create_from_addr(addr);
		return uq;
	}
	bool query(Unique_Key &uq_key_in, string & data_out) {
		return true;
	}
	bool store(Storable & object_in, Unique_Key & key_out) {
		return true;
	}
	bool delete_storable(Storable & object_in) {
		return true;
	}
	bool delete_storable(Unique_Key & key_in) {
		return true;
	}
	Storable & 	find_by_key(Unique_Key & key_in) {
		StorableTest val;
		return val;
	}
	void init_test();
	void TestA();

public:
	StorableTest()
    {
		TEST_ADD(StorableTest::TestA)
		//TEST_ADD(StorableTest::TestB)
        // TEST_ADD(EndianTest::second_test)
    }
    ~StorableTest() {

    }
};
void StorableTest::init_test()
{

    TEST_ASSERT(1 == 1);
}
void StorableTest::TestA() {
	StorableTest strbl;
	Unique_Key uq = strbl.calc_key(strbl);

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

int main(int argc, char ** argv) {
	Test::TextOutput output(Test::TextOutput::Verbose);
	StorableTest ts;
	return ts.run(output) ? EXIT_SUCCESS : EXIT_FAILURE;
}




