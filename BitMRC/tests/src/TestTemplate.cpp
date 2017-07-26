/*
 * TestTemplate.cpp
 *

 */
#include <cpptest.h>
// include what to test
//#include <Storage/Storage_sqlite3.h>

#include <types.h>
class TestTemplate : public Test::Suite
{

private:
    void test_01();
public:
    TestTemplate()
    {
        TEST_ADD(TestTemplate::test_01)
    }
    ~TestTemplate() {

    }
};

void TestTemplate::test_01() {
	bool ret = false;
	TEST_ASSERT(ret == true);
}
int main(int argc, char ** argv) {
	Test::TextOutput output(Test::TextOutput::Verbose);
	TestTemplate tst;
	return tst.run(output) ? EXIT_SUCCESS : EXIT_FAILURE;
}




