/*
 * TestTemplate.cpp
 *

 */
#include <cpptest.h>
#include <Storage/ColumnSpec.h>

#include <types.h>
class ColumnSpecTest : public Test::Suite
{

private:
    void test_create_01();
    void test_create_02();

public:
    ColumnSpecTest()
    {
        TEST_ADD(ColumnSpecTest::test_create_01)
    	TEST_ADD(ColumnSpecTest::test_create_02)
    }
    ~ColumnSpecTest() {

    }
};

void ColumnSpecTest::test_create_01() {
	bool ret = false;
	ColumnSpec * spec = new ColumnSpec(CS_TYPE_NONE, "", 100);
	TEST_ASSERT(spec->getLen() == 100);
	TEST_ASSERT(spec->getType() == CS_TYPE_NONE);
	delete spec;

}
void ColumnSpecTest::test_create_02() {
	bool ret = false;
	ColumnSpec * spec = new ColumnSpec(CS_TYPE_MAX, "", 101);
	TEST_ASSERT(spec->getLen() == 101);
	TEST_ASSERT(spec->getType() == CS_TYPE_NONE);
	delete spec;
}
int main(int argc, char ** argv) {
	Test::TextOutput output(Test::TextOutput::Verbose);
	ColumnSpecTest tst;
	return tst.run(output) ? EXIT_SUCCESS : EXIT_FAILURE;
}




