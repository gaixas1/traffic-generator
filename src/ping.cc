#include "ping.h"


#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <chrono>
#include <thread>


#ifndef RINA_PREFIX
#define RINA_PREFIX "ping"
#endif // !RINA_PREFIX

#include <librina/librina.h>
#include <librina/logs.h>

using namespace std;
using namespace std::chrono;
using namespace std::this_thread;
using namespace rina;

struct data_t {
	long ping;
	int seq;
};

bool readBuffer(int fd, char * buffer, int i) {
	int padding = 0;
	int ret;
	do {
		try {
			ret = read(fd, buffer + padding, i);
		} catch (...) {
			return false;
		}
		if (ret < 0) {
			LOG_ERR("read() failed: %s", strerror(errno));
			return false;
		}
		i -= ret;
		padding += ret;
	} while (i > 0);
	return true;
}


void ping_server::handle_flow(int port_id, int fd) {
	cout << "ping server - Handle flow "<< port_id << ", "<< fd<<endl;
	data_t data;
	char * buffer = (char*)& data;

	for (;;) {
		if (readBuffer(fd, buffer, sizeof(data_t))) {
			try {
				if (write(fd, buffer, sizeof(data_t)) != sizeof(data_t)) {
					LOG_ERR("FAILED AT ECHO - ABORT FLOW");
					release_flow(port_id);
					return;
				}
			} catch (...) { return; }
		} else {
			LOG_ERR("FAILED AT READ - ABORT FLOW");
			release_flow(port_id);
			return;
		}
		
		if(data.seq <= 0) {
			return;
		}
	}
}



void ping_client::handle_flow(int port_id, int fd) {
	cout << "ping client - Handle flow "<< port_id << ", "<< fd<<endl;
	data_t data;
	char * buffer = (char*)& data;
	srand(time(0));

	long t0 = duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count();
	
	for (int i = MAX_MSG; i >= 0; i--) {
		cout << "--- Send msg : " << (MAX_MSG-i)<< endl;
		data.seq = i;
		data.ping = duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count();
		
		try {
			if (write(fd, buffer, sizeof(data_t)) != sizeof(data_t)) {
				LOG_ERR("FAILED AT ECHO - ABORT FLOW");
				break;
			}
		} catch (...) { return; }

		if (readBuffer(fd, buffer, sizeof(data_t))) {
			long t1 = duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count();
			double dif = (t1 - data.ping)/1000000.0;

			cout << "Pong received after " << dif << " ms" << endl;
		} else {
			LOG_ERR("FAILED AT READ - ABORT FLOW");
			break;
		}
	}
	long t1 = duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count();
	
	cout << "Test duration " << ((t1-t0)/1000000.0) << " ms || "<< t0 << " -> "<< t1 << endl;
	
	release_flow(port_id);
}


