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
 * Configuration.h
 *
 *  Created on: 11.09.2016
 *      Author: steady286
 */

#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_
using namespace std;
#include <Storage/Storable.h>
#include <Storage/Unique_Key.h>
#include <string.h>

typedef struct  {
	uint32_t node_id;
	uint32_t working_mode;
	uint32_t networking_flags;
	uint32_t stream_id1;
	uint32_t stream_id2;
	uint32_t stream_id3;
	uint32_t stream_id4;
	uint32_t last_startup_timestamp;
	uint32_t last_startup_result;
	uint32_t database_version;
} system_config_t;

typedef struct {
	uint16_t settingsversion; /* \todo: check ranges */
	uint16_t port;
	uint8_t keys_encrypted;
	uint8_t messages_encrypted;
	uint16_t defaultnoncetrialsperbyte;
	uint16_t defaultpayloadlengthextrabytes;
	uint32_t maxacceptablenoncetrialsperbyte;
	uint16_t maxdownloadrate_kbyte_s;
	uint16_t maxuploadratekbyte_s;
	uint16_t stopresendingafterxdays;
	uint16_t stopresendingafterxmonths;
	uint16_t ttl;
	uint16_t version;
	unsigned char lastvacuumtime[16];
	unsigned char user_locale[12]; /* ISO_XXXX_YY */
	unsigned char timeformat[32];
} settings_config_t;
class Configuration {

public:
	Configuration();
	~Configuration();
public:
	/** \todo: add settings_config functions */
	bool is_valid();
	uint32_t get_node_id();
	void set_node_id(uint32_t id_in);
	uint32_t get_working_mode();
	void set_working_mode(uint32_t mode_in);
	uint32_t get_networking_flags();
	void set_networking_flags(uint32_t flags_in);
	uint32_t get_stream_id1();
	void set_stream_id1(uint32_t stream_in);
	uint32_t get_stream_id2();
	void set_stream_id2(uint32_t stream_in);
	uint32_t get_stream_id3();
	void set_stream_id3(uint32_t stream_in);
	uint32_t get_stream_id4();
	void set_stream_id4(uint32_t stream_in);
	uint32_t get_last_startup_time();
	void set_last_startup_time(uint32_t time_in);
	uint32_t get_last_startup_result();
	void set_last_startup_result(uint32_t res_in);
	void set_database_version(uint32_t v_in);
	uint32_t get_database_version();
	void set(Configuration & cfg);  /** set the values from another config element */
	void print();

	void set_valid();
	void set_invalid();
protected:
	Storage * st;
	bool valid;
	system_config_t system;
	settings_config_t settings;
};




#endif /* CONFIGURATION_H_ */
