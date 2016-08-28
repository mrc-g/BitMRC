// BitMRC Utils Lib Tests
// TBC ..
#include <cpptest.h>
#include <utils.h>
#include <string.h>
#include <bitmrc_endian.h>
class UtilTest : public Test::Suite
{
public:
	UtilTest()
    {
        TEST_ADD(UtilTest::init_test)
		TEST_ADD(UtilTest::tohex)
    }

private:
    void init_test();
	void tohex();
};
void UtilTest::init_test() {
	    TEST_ASSERT(1 == 1);
}
void UtilTest::tohex() {
	ustring us;
	const unsigned char hex[] = {"|61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f 70 |abcdefghijklmnop|\n|71 72 73 74 75 76 77 78 79 7a 30 31 32 33 34 35 |qrstuvwxyz012345|\n|36 37 38 39 |6789|\n"};
	const unsigned char hex2[] = {"|01 02 03 04 |....|\n"};

	us.assign("abcdefghijklmnopqrstuvwxyz0123456789");
	string out = us.toHexString();
	printf("out string is (%u)\n%s\n", strlen(out.c_str()), out.c_str() );
	printf("base string is (%u)\n%s\n", strlen(hex), hex );
	TEST_ASSERT(strcmp(hex,out.c_str()) == 0);

	us.assign("\1\2\3\4");
	out = us.toHexString();
	printf("out string is (%u)\n%s\n", strlen(out.c_str()), out.c_str() );
	printf("base string2 is (%u)\n%s\n", strlen(hex2), hex2 );

	TEST_ASSERT(strcmp(hex2,out.c_str()) == 0);
}

int main(int argc, char ** argv) {
	Test::TextOutput output(Test::TextOutput::Verbose);
	UtilTest ts;
	return ts.run(output) ? EXIT_SUCCESS : EXIT_FAILURE;
}
