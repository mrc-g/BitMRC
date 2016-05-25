/*
 * thread_test.h
 *
 *  Created on: 23.05.2016
 *      Author: blueboxes
 */

#ifndef THREAD_TEST_H_
#define THREAD_TEST_H_

class thread_test {
private:
	thread thread_new_packets;
	thread thread_new_inv;
public:
	static int running;
public:
	static bool is_running();
	static void set_running();
	void tfunc();
	thread_test();
	~thread_test();
	void start();
	void stop();
};




#endif /* THREAD_TEST_H_ */
