#include "clocksincro.h"

#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <chrono>
#include <thread>

#ifndef RINA_PREFIX
#define RINA_PREFIX "CLOCKSINCRO"
#endif // !RINA_PREFIX

#include <librina/librina.h>
#include <librina/logs.h>

using namespace std;
using namespace std::chrono;
using namespace std::this_thread;
using namespace rina;

#define DATA_SIZE 32

struct data_t {
	long ping, pong;
	int seq;
};

void clocksincro_server::handle_flow(int port_id, int fd) {
	data_t data;
	char * buffer = (char*)& data;
	try {
		do{
			if (readBuffer(fd, buffer, DATA_SIZE) != 0) {
				LOG_ERR("FAILED AT READ - ABORT FLOW");
				break;
			}
			
			data.pong = duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count();
			
			if (write(fd, buffer, DATA_SIZE) != DATA_SIZE) {
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



void clocksincro_client::handle_flow(int port_id, int fd) {
	data_t data;
	char * buffer = (char*)& data;
	srand(time(0));

	long minLat = LONG_MAX;
	long minDif = LONG_MAX;
	data.seq = MAX_MSG;
	
	try {
		while(data.seq-- > 0) {
			if(MAX_SLEEP > 0){
				sleep_for( milliseconds( rand() % MAX_SLEEP ) );
			}
			
			data.ping = duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count();
			
			if (write(fd, buffer, DATA_SIZE) != DATA_SIZE) {
				LOG_ERR("FAILED AT ECHO - ABORT FLOW");
				break;
			}
			
			if (readBuffer(fd, buffer, DATA_SIZE) != 0) {
				LOG_ERR("FAILED AT READ - ABORT FLOW");
				break;
			}
			
			long t1 = duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count();
			long lat = (t1 - data.ping) / 2;
			if (lat < minLat) { minLat = lat; }

			long difa = (data.pong - data.ping);
			if (difa < minDif) { minDif = difa; }
			
			long difb = (t1 - data.pong);
			if (difb < minDif) { minDif = difb; }
		}
		release_flow(port_id);
	} catch(...){
		LOG_ERR("EXCEPTION CATCHED - Something failed");
	}
	
	cout << (minLat - minDif) << endl;
	
}


