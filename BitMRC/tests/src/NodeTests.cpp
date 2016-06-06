/*
 * NodeTests.cpp
 *
 *  Created on: 06.05.2016
 *      Author: steady286
 *      tests on nodeConnecton and NodeBlacklist classes
 */

// a simple test as a start for BitMRC
// testing environment
// https://github.com/steady286
#include <cpptest.h>
#include <NodeBlacklist.h>
class NodeTest : public Test::Suite
{

private:
    void init_test();
	void BlacklistTest();
	void BlacklistTestBin();
    //void second_test();
	NodeBlacklist * bl;
public:
    NodeTest()
    {
        TEST_ADD(NodeTest::init_test)
		TEST_ADD(NodeTest::BlacklistTest)
		TEST_ADD(NodeTest::BlacklistTestBin)
        // TEST_ADD(EndianTest::second_test)
    }
    ~NodeTest() {
    	delete bl;
    }
};
void NodeTest::init_test()
{
	bl = new(NodeBlacklist);
    TEST_ASSERT(1 == 1);
}
void NodeTest::BlacklistTest() {

	TEST_ASSERT(bl->test() == 0);

}
void NodeTest::BlacklistTestBin() {

	TEST_ASSERT(bl->test_binaddr() == 1);

}
#if 0
void ExampleTestSuite::second_test()
{
}
#endif

int main(int argc, char ** argv) {
	Test::TextOutput output(Test::TextOutput::Verbose);
	NodeTest ts;
	return ts.run(output) ? EXIT_SUCCESS : EXIT_FAILURE;
}




