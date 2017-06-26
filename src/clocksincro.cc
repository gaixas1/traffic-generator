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

struct data_t {
	long ping, pong;
};

bool readBuffer(int fd, char * buffer, int i) {
	int padding = 0;
	int ret;
	do {
		ret = read(fd, buffer + padding, i);
		if (ret < 0) {
			LOG_ERR("read() failed: %s", strerror(errno));
			return false;
		}
		i -= ret;
		padding += ret;
	} while (i > 0);
	return true;
}


void clocksincro_server::handle_flow(int port_id, int fd) {
	cout << "Clocksincro server - Handle flow "<< port_id << ", "<< fd<<endl;
	data_t data;
	char * buffer = (char*)& data;

	for (;;) {
		if (readBuffer(fd, buffer, sizeof(data_t))) {
			data.pong = duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count();
			if (write(fd, buffer, sizeof(data_t)) != sizeof(data_t)) {
				LOG_ERR("FAILED AT ECHO - ABORT FLOW");
				release_flow(port_id);
				return;
			}
		} else {
			LOG_ERR("FAILED AT READ - ABORT FLOW");
			release_flow(port_id);
			return;
		}
	}
}



void clocksincro_client::handle_flow(int port_id, int fd) {
	cout << "Clocksincro client - Handle flow "<< port_id << ", "<< fd<<endl;
	data_t data;
	char * buffer = (char*)& data;
	srand(time(0));

	long minLat = LONG_MAX;
	long minDif = LONG_MAX;

	for (int i = 0; i < MAX_MSG; i++) {
		int ms = rand() % MAX_SLEEP;
		sleep_for(milliseconds(ms));
		data.ping = duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count();
		if (write(fd, buffer, sizeof(data_t)) != sizeof(data_t)) {
			LOG_ERR("FAILED AT ECHO - ABORT FLOW");
			break;
		}

		if (readBuffer(fd, buffer, sizeof(data_t))) {
			long t1 = duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count();
			long lat = (t1 - data.ping) / 2;
			if (lat < minLat) { minLat = lat; }

			long dif = (data.pong - data.ping);
			if (dif < minDif) { minDif = dif; }

			cout << (minLat-minDif) << endl;
		} else {
			LOG_ERR("FAILED AT READ - ABORT FLOW");
			break;
		}
	}
	cout << (minLat - minDif) << endl;
	release_flow(port_id);
}


