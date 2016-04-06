// a simple test as a start for BitMRC 
// testing environment
// https://github.com/steady286
#include <cpptest.h>
#include <bitmrc_endian.h>
class EndianTest : public Test::Suite
{
public:
    EndianTest()
    {
        TEST_ADD(EndianTest::init_test)
		TEST_ADD(EndianTest::shortswap)
        // TEST_ADD(EndianTest::second_test)
    }
    
private:
    void init_test();
	void shortswap();
    //void second_test();
};
void EndianTest::init_test()
{
	InitEndian();
    TEST_ASSERT(1 == 1);
}
void EndianTest::shortswap() {

	short a,b,c;
	a = 0x1234;
	b = 0x3412;
	c = ShortSwap(a);
	TEST_ASSERT(c==b);
}
#if 0
void ExampleTestSuite::second_test()
{
}
#endif

int main(int argc, char ** argv) {
	Test::TextOutput output(Test::TextOutput::Verbose);
	EndianTest ts;
	return ts.run(output) ? EXIT_SUCCESS : EXIT_FAILURE;
}
