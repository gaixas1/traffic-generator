#include "data_client.h"
#include <fstream>
#include <thread>
#include <stdlib.h>
#include <time.h>


#ifndef RINA_PREFIX
#define RINA_PREFIX "DATA_CLIENT"
#endif // !RINA_PREFIX

#include <librina/librina.h>
#include <librina/logs.h>
#include "common.h"

using namespace std;
using namespace std::chrono;
using namespace std::this_thread;
using namespace rina;

void data_client::setBurstSize(int min, int max) {
	if(min < 1) { min = 1;}
	if(max < 1) { max = 1;}
	if (min == max) {
		MIN_BURST = min;
		DIF_BURST = 0;
	} else if (min < max) {
		MIN_BURST = min;
		DIF_BURST = max - min;
	} else {
		MIN_BURST = max;
		DIF_BURST = min - max;
	}
}

void data_client::setData(int kB) {
	if(kB < 1) { kB = 1; }
	MIN_DATA = kB;
}

bool data_client::flow(int fd, char * buffer) {
	dataSDU * data = (dataSDU*)buffer;

	nanoseconds timeD(timeDIF);
	data->type = DTYPE_DATA;
	int rem_burst = MIN_BURST + (DIF_BURST > 0 ? rand() % DIF_BURST : 0);
	auto t = system_clock::now() + nanoseconds(rem_burst * nsPDU);
	
	int rem = (MIN_DATA * 2000) / (2 * MIN_PDU + DIF_PDU);
	for(; rem > 0; rem--) {
		data->size = MIN_PDU + (DIF_PDU > 0? rand() % DIF_PDU : 0);
		data->seqId++;
		data->ping_time = duration_cast<milliseconds>(system_clock::now().time_since_epoch() - timeD).count();

		try {
			if (write(fd, buffer, data->size) != (int)data->size) {
				LOG_ERR("FAILED AT SENDING DATA MESSAGE - ABORT FLOW");
				return false;
			}
		} catch(...){
			LOG_ERR("EXCEPTION CATCHED - Write failed");
			return false;
		}

		if (--rem_burst == 0) {
			if (busyWait) {
				while (system_clock::now() < t) {}
			} else {
				if (t > system_clock::now()) { sleep_until(t); }
			}
		} else {
			rem_burst = MIN_BURST + (DIF_BURST > 0? rand() % DIF_BURST : 0);
			t += nanoseconds(rem_burst * nsPDU);
		}
	}

	return true;
}
