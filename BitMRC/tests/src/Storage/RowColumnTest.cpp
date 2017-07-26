/*
 * RowColumnTest.cpp
 * steady286
 */
#include <cpptest.h>
// include what to test
#include <Storage/ColumnSpec.h>
#include <Storage/RowSpec.h>
#include <string.h>
#include <types.h>
class RowColumnTest : public Test::Suite
{

private:
    void test_01();
    void test_idx();
public:
    RowColumnTest()
    {
        TEST_ADD(RowColumnTest::test_01)
        TEST_ADD(RowColumnTest::test_idx)
    }
    ~RowColumnTest() {

    }
};

void RowColumnTest::test_01() {

	bool ret = false;
	RowSpec * row = new RowSpec();



	ret = row->addColumn(ColumnSpec(CS_TYPE_INT16, "int16value", 0));
	TEST_ASSERT(ret == true);
	row->addColumn(ColumnSpec(CS_TYPE_STRING, "stringval", 32));
	TEST_ASSERT(ret == true);
	TEST_ASSERT(row->getColumnCount() == 2);
	delete row;

}
void RowColumnTest::test_idx() {

	bool ret = false;
	RowSpec * row = new RowSpec();

	ret = row->addColumn(ColumnSpec(CS_TYPE_INT16, "int16value", 0));
	TEST_ASSERT(ret == true);
	row->addColumn(ColumnSpec(CS_TYPE_STRING, "stringval", 32));
	TEST_ASSERT(ret == true);
	row->addColumn(ColumnSpec(CS_TYPE_INT64, "int64val", 0));
	TEST_ASSERT(ret == true);
	row->addColumn(ColumnSpec(CS_TYPE_BLOB, "blobval", 128));
	TEST_ASSERT(ret == true);

	ColumnSpec * col = NULL;
	TEST_ASSERT(row->getColumnCount() == 4);

	col = row->operator[](0);
	TEST_ASSERT(col != NULL);
	TEST_ASSERT(col->getType() == CS_TYPE_INT16);
	TEST_ASSERT(strcmp(col->getName(),"int16value") == 0);
	TEST_ASSERT(col->getLen() == 0);

	col = row->operator[](1);
	TEST_ASSERT(col != NULL);
	TEST_ASSERT(col->getType() == CS_TYPE_STRING);
	TEST_ASSERT(strcmp(col->getName(),"stringval") == 0);
	TEST_ASSERT(col->getLen() == 32);

	col = row->operator[](2);
	TEST_ASSERT(col != NULL);
	TEST_ASSERT(col->getType() == CS_TYPE_INT64);
	TEST_ASSERT(strcmp(col->getName(),"int64val") == 0);
	TEST_ASSERT(col->getLen() == 0);

	col = row->operator[](3);
	TEST_ASSERT(col != NULL);
	TEST_ASSERT(col->getType() == CS_TYPE_BLOB);
	TEST_ASSERT(strcmp(col->getName(),"blobval") == 0);
	TEST_ASSERT(col->getLen() == 128);

	delete row;

}
int main(int argc, char ** argv) {
	Test::TextOutput output(Test::TextOutput::Verbose);
	RowColumnTest tst;
	return tst.run(output) ? EXIT_SUCCESS : EXIT_FAILURE;
}




