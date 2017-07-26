/*
* Copyright (c) 2016 steady286 github@dutka-is.de
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
* OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
* OTHER DEALINGS IN THE SOFTWARE.
*/
/*
 * StorablePacketTest
 * generate some to-be-saved packets and check their storage into a sql-ish storage
 *
 *
 */
#include <cpptest.h>// include what to test
#include <Addr.h>
#include <Storage/Storage_mysql.h>

#include <types.h>
class StorablePacketTest : public Test::Suite
{

private:
	void test_init();
    void test_addr();
    void test_pubkey();
public:
    StorablePacketTest()
    {
    	TEST_ADD(StorablePacketTest::test_init)
        TEST_ADD(StorablePacketTest::test_addr)
		TEST_ADD(StorablePacketTest::test_pubkey)
    }
    ~StorablePacketTest() {

    }
    Storage_mysql * strg;
};
void StorablePacketTest::test_init() {
	uint32_t ret = 12;
	bool bret = false;
	strg = new Storage_mysql();
	TEST_ASSERT(strg != NULL);
	if(strg!=NULL) {
		bret = strg->open("bitmrc_test", "bitmrc", "BitMRC");
		TEST_ASSERT(bret == true);
		bret = strg->drop_tables();
		TEST_ASSERT(bret == true);
		bret = strg->create_tables();
		TEST_ASSERT(bret == true);
	}
}
/* generate an address, save the public part
 * then generate a pubkey from its public part, and save that
 * in conjunction with the pub address.
 */
void StorablePacketTest::test_pubkey() {
	bool ret = false;
	Unique_Key k;
	packet_pubkey p;
	char * cret = NULL;
	TEST_ASSERT(ret == false);

	PubAddr ad;
#if 0
	// tbc
	bret = ad.generateRandom();
	TEST_ASSERT(bret == true);
	int i = 0, nonce = 0;

	ustring pub_addr = ad.buildAddressFromKeys(ad.getPubSigningKey(), ad.getPubEncryptionKey(), ad.getStream(), ad.getVersion());
	//p.decodeFromObj(packet_pubkey pubkey);
	cout << "PubEnc Key" << endl << ad.getPubOfPriv(ad.getPrivEncryptionKey()).toHexString().c_str() << endl;
	cout << "PubSign Key" << endl << ad.getPubOfPriv(ad.getPrivSigningKey()).toHexString().c_str() << endl;
#endif
}
void StorablePacketTest::test_addr() {
	// base packet class just returns false for all storable i/f methods
	bool ret = false;
	Unique_Key k;
	Packet p;
	char * cret = NULL;
	std::string data_out;

	ret = p.set_key(k);
	TEST_ASSERT(ret == false);

	ret = p.store();
	TEST_ASSERT(ret == false);
	ret = p.query(k, data_out);
	TEST_ASSERT(ret == false);
	ret = p.delete_storable(p);
	TEST_ASSERT(ret == false);
	ret = p.delete_storable(k);
	TEST_ASSERT(ret == false);
	ret = p.find_by_key(k);
	TEST_ASSERT(ret == false);
	ret = p.set_storage(strg);
	TEST_ASSERT(ret == false);

	cret = p.get_update_query();
	TEST_ASSERT(cret == NULL);
	cret = p.get_load_query();
	TEST_ASSERT(cret == NULL);
}
int main(int argc, char ** argv) {
	Test::TextOutput output(Test::TextOutput::Verbose);
	StorablePacketTest tst;
	return tst.run(output) ? EXIT_SUCCESS : EXIT_FAILURE;
}
#if 0
bool set_key(Unique_Key & key_in); // set the unique key manually
Unique_Key get_key();
virtual bool query(Unique_Key &uq_key_in, std::string & data_out) = 0;
virtual bool store() = 0;
virtual bool delete_storable(Storable & object_in) = 0;
virtual bool delete_storable(Unique_Key & key_in) = 0;
virtual bool find_by_key(Unique_Key &) = 0;
bool set_storage(Storage*);
virtual char * get_update_query();
virtual char * get_load_query();
#endif



