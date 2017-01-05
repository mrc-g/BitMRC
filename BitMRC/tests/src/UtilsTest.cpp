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
		TEST_ADD(UtilTest::hashToASCII)
    }
	class TestDescr {

	};
private:
    void init_test();
	void tohex();
	void hashToASCII();
};
void UtilTest::init_test() {
	    TEST_ASSERT(1 == 1);
}
void UtilTest::hashToASCII () {
	/* check some manually crafted values */
	unsigned char test_val[] = {0x00, 0x34, 0x35, 0x36, 0x37, 0x00};
	ustring converter;
	string ret = converter.toHexString("0123456789abcdef", 16);
	TEST_ASSERT(ret.length() == 32);
	TEST_ASSERT(ret == "30313233343536373839616263646566");
	cout << "test value is : "<< ret.c_str() << " length " << ret.length() << endl;

	ret = converter.toHexString(test_val,6);
	TEST_ASSERT(ret.length() == 12);
	TEST_ASSERT(ret == "003435363700");

	cout << "test value is : "<< ret.c_str() << " length " << ret.length() << endl;

}
void UtilTest::tohex() {
	ustring us;
	const char hex[] = {"|61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f 70 |abcdefghijklmnop|\n|71 72 73 74 75 76 77 78 79 7a 30 31 32 33 34 35 |qrstuvwxyz012345|\n|36 37 38 39 |6789|\n"};
	const char hex2[] = {"|01 02 03 04 |....|\n"};

	us.assign("abcdefghijklmnopqrstuvwxyz0123456789");
	string out = us.toHexString();
	printf("out string is (%lu)\n%s\n", strlen(out.c_str()), out.c_str() );
	printf("base string is (%lu)\n%s\n", strlen(hex), hex );
	TEST_ASSERT(strcmp(hex,out.c_str()) == 0);

	us.assign("\1\2\3\4");
	out = us.toHexString();
	printf("out string is (%lu)\n%s\n", strlen(out.c_str()), out.c_str() );
	printf("base string2 is (%lu)\n%s\n", strlen(hex2), hex2 );

	TEST_ASSERT(strcmp(hex2,out.c_str()) == 0);
}

int main(int argc, char ** argv) {
	Test::TextOutput output(Test::TextOutput::Verbose);
	UtilTest ts;
	return ts.run(output) ? EXIT_SUCCESS : EXIT_FAILURE;
}
