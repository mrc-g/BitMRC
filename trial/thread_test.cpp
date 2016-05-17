/*
 * thread_test.cpp
 *
 *  Created on: 16.05.2016
 *      Author: steady286
 */
#include <thread>
#ifdef LINUX
#include <unistd.h>
#endif
using namespace std;
class thread_test {
private:
	thread thread_new_packets;
	thread thread_new_inv;
public:
	static int running ;
public:
	static bool is_running() {
		return (thread_test::running != 0);
	}
	static void set_running() {
		thread_test::running = 1;
	}

	void tfunc() {
		while(thread_test::is_running()) {
			usleep(100000);
		}
	}
	thread_test() {
		set_running();
		thread_new_packets = thread(&thread_test::tfunc, this);
		thread_new_packets = thread(&thread_test::tfunc, this);

	}
	~thread_test() {

	}
	void start() {

	}
	void stop() {

	}
};

int main(int argc, char ** argv) {

	thread_test t;
	t.start();
	usleep(1000000);
	t.stop();
	return 0;
}
