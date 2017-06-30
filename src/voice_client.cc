#include "voice_client.h"
#include <fstream>
#include <thread>
#include <stdlib.h>
#include <time.h>

#ifndef RINA_PREFIX
#define RINA_PREFIX "VOICE_CLIENT"
#endif // !RINA_PREFIX

#include <librina/librina.h>
#include <librina/logs.h>
#include "common.h"

using namespace std;
using namespace std::chrono;
using namespace std::this_thread;
using namespace rina;

void voice_client::setON(int min_ms, int max_ms) {
	if (min_ms == max_ms) {
		MIN_ON = min_ms;
		DIF_ON = 0;
	}
	else if (min_ms < max_ms) {
		MIN_ON = min_ms;
		DIF_ON = max_ms - min_ms;
	}
	else {
		MIN_ON = max_ms;
		DIF_ON = min_ms - max_ms;
	}
	if (MIN_ON <= 0) {
		throw std::invalid_argument("received negative value for ON interval");
	}
}
void voice_client::setOFF(int min_ms, int max_ms) {
	if (min_ms == max_ms) {
		MIN_OFF = min_ms;
		DIF_OFF = 0;
	}
	else if (min_ms < max_ms) {
		MIN_OFF = min_ms;
		DIF_OFF = max_ms - min_ms;
	}
	else {
		MIN_OFF = max_ms;
		DIF_OFF = min_ms - max_ms;
	}
	if (MIN_OFF <= 0) {
		throw std::invalid_argument("received negative value for OFF interval");
	}
}

void voice_client::setDuration(int ms) {
	duration = ms;
}

bool voice_client::flow(int fd, char * buffer) {
	dataSDU * data = (dataSDU*)buffer;
	nanoseconds timeD(timeDIF);
	data->type = DTYPE_DATA;
	
	if(MIN_OFF > 0) { sleep_for(milliseconds(rand() % MIN_OFF)); }
	if(nsPDU <= 0) { nsPDU = 1; }
	auto tend = system_clock::now() + milliseconds(duration);
	
	do {
		int rem = ( MIN_ON + (DIF_ON > 0 ? rand() % DIF_ON : 0) ) * 1000000 / nsPDU;
		auto t = system_clock::now() + timeD;
		for (;rem > 0; rem--) {
			data->type = DTYPE_DATA;
			data->size = MIN_PDU + (DIF_PDU > 0? rand() % DIF_PDU : 0);
			data->seqId++;
			data->ping_time = duration_cast<milliseconds>(system_clock::now().time_since_epoch() - timeD).count();

			try {
				if (write(fd, buffer, data->size) != data->size) {
					LOG_ERR("FAILED AT SENDING DATA MESSAGE - ABORT FLOW");
					return false;
				}
			} catch(...){
				LOG_ERR("EXCEPTION CATCHED - Write failed");
				return false;
			}

			if (busyWait) {
				while(system_clock::now() < t) {}
			} else if (t > system_clock::now()) {
				sleep_until(t);
			}
			t += timeD;
		}
		sleep_for(milliseconds(MIN_OFF + (DIF_OFF > 0? rand() % DIF_OFF : 0) ));
	} while (system_clock::now() <= tend);
	return true;
}
