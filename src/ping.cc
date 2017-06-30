#include "ping.h"

#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <chrono>
#include <thread>

#ifndef RINA_PREFIX
#define RINA_PREFIX "PING"
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

void ping_server::handle_flow(int port_id, int fd) {
	data_t data;
	char * buffer = (char*)& data;
	try {
		do{
			if (readBuffer(fd, buffer, sizeof(data_t)) != 0) {
				LOG_ERR("FAILED AT READ - ABORT FLOW");
				break;
			}
			if (write(fd, buffer, sizeof(data_t)) != sizeof(data_t)) {
				LOG_ERR("FAILED AT ECHO - ABORT FLOW");
				break;
			}
		} while(data.seq > 0);
	} catch(...){
		LOG_ERR("EXCEPTION CATCHED - Something failed");
		try {
			release_flow(port_id);
		} catch(...){
			LOG_ERR("EXCEPTION CATCHED - Something failed while releasing flow");
		}
	}
}

void ping_client::handle_flow(int port_id, int fd) {
	data_t data;
	char * buffer = (char*)& data;
	srand(time(0));
	long t0 = duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count();
	
	data.seq = MAX_MSG;
	
	try {
		while(data.seq-- > 0) {
			cout << "--- Send msg : " << (MAX_MSG-data.seq)<< endl;
			data.ping = duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count();
			
			if (write(fd, buffer, sizeof(data_t)) != sizeof(data_t)) {
				LOG_ERR("FAILED AT ECHO - ABORT FLOW");
				break;
			}
			
			if (readBuffer(fd, buffer, sizeof(data_t)) != 0) {
				LOG_ERR("FAILED AT READ - ABORT FLOW");
				break;
			}
			
			long t1 = duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count();
			double dif = (t1 - data.ping)/1000000.0;
			cout << "Pong received after " << dif << " ms" << endl;
		}
		release_flow(port_id);
	} catch(...){
		LOG_ERR("EXCEPTION CATCHED - Something failed");
	}
	
	long t1 = duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count();
	cout << "Test duration " << ((t1-t0)/1000000.0) << " ms || "<< t0 << " -> "<< t1 << endl;
}


