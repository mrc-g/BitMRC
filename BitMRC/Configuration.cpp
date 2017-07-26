/*
 * Configuration.cpp
 *
 *  Created on: 11.09.2016
 *      Author: steady286
 *      Encapsulate all config options.
 *      Provide get/set methods for all relevant options
 *
 */
#include <Configuration.h>
#include <Storage/Storage.h>
#include <string.h>
#include <Storage/Unique_Key.h>
#include <Storage/QueryResult.h>
#include <iostream>

using namespace std;

Configuration::Configuration() {
	this->st = NULL;
	this->valid = false;
	memset(&this->system, 0, sizeof(system_config_t));
	memset(&this->settings, 0, sizeof(settings_config_t));
}
Configuration::~Configuration() {

}
void Configuration::set_valid() {
	this->valid = true;
}
void Configuration::set_invalid() {
	this->valid = false;
}
void Configuration::set(Configuration & cfg) {
	this->system = cfg.system;
	this->settings = cfg.settings;
	this->valid = true;
}
uint32_t Configuration::get_node_id() {
	return system.node_id;
}
void Configuration::set_node_id(uint32_t id_in) {
	system.node_id = id_in;
	set_valid();
}
uint32_t Configuration::get_working_mode() {
	return system.working_mode;
}
void Configuration::set_working_mode(uint32_t mode_in){
	system.working_mode = mode_in;
}
uint32_t Configuration::get_networking_flags() {
	return system.networking_flags;
}
void Configuration::set_networking_flags(uint32_t flags_in) {
	system.networking_flags = flags_in;
}
uint32_t Configuration::get_stream_id1() {
	return system.stream_id1;
}
void Configuration::set_stream_id1(uint32_t stream_in) {
	system.stream_id1 = stream_in;
}
uint32_t Configuration::get_stream_id2() {
	return system.stream_id2;
}
void Configuration::set_stream_id2(uint32_t stream_in) {
	system.stream_id4 = stream_in;
}
uint32_t Configuration::get_stream_id3() {
	return system.stream_id3;
}
void Configuration::set_stream_id3(uint32_t stream_in) {
	system.stream_id3 = stream_in;
}
uint32_t Configuration::get_stream_id4() {
	return system.stream_id4;
}
void Configuration::set_stream_id4(uint32_t stream_in) {
	system.stream_id4 = stream_in;
}
uint32_t Configuration::get_last_startup_time() {
	return system.last_startup_timestamp;
}
void Configuration::set_last_startup_time(uint32_t time_in) {
	system.last_startup_timestamp = time_in;
}
uint32_t Configuration::get_last_startup_result() {
	return system.last_startup_timestamp;
}
void Configuration::set_last_startup_result(uint32_t res_in) {
	system.last_startup_result = res_in;
}
bool Configuration::is_valid() {
	return this->valid;
}

void Configuration::set_database_version(uint32_t v_in) {
	system.database_version = v_in;
}
uint32_t Configuration::get_database_version() {
	return system.database_version;
}

void Configuration::print() {
	cout << "=== Configuration ==" << endl;
	cout << " NodeID " << this->system.node_id << endl;
	cout << " Networking Flags " << this->system.networking_flags << endl;
	cout << " Stream 1 " << this->system.stream_id1 << endl;
	cout << " Stream 2 " << this->system.stream_id2 << endl;
	cout << " Stream 3 " << this->system.stream_id3 << endl;
	cout << " Stream 4 " << this->system.stream_id4 << endl;
	cout << " Database Version " << this->system.database_version << endl;
	cout << " Last Startup time " << this->system.last_startup_timestamp << endl;
	cout << " Last Startup result " << this->system.last_startup_result	 << endl;
}

