#include "data_client.h"
#include <fstream>
#include <thread>
#include <stdlib.h>
#include <time.h>

#ifndef RINA_PREFIX
#define RINA_PREFIX "DATA_CLIENT"
#endif // !RINA_PREFIX
#include "common.h"

using namespace std;
using namespace std::chrono;
using namespace std::this_thread;
using namespace rina;

void data_client::setBurstSize(int min, int max) {
	if (min == max) {
		MIN_BURST = min;
		DIF_BURST = 0;
	}
	else if (min < max) {
		MIN_BURST = min;
		DIF_BURST = max - min;
	}
	else {
		MIN_BURST = max;
		DIF_BURST = min - max;
	}
	if (MIN_BURST <= 0) {
		throw std::invalid_argument("received negative value for burst size");
	}
}

void data_client::setData(int kB) {
	MIN_DATA = kB;
}


bool data_client::flow(int fd, char * buffer) {
	dataSDU * data = (dataSDU*)buffer;
	int remaining = (MIN_DATA * 2000) / (2 * MIN_PDU + DIF_PDU);

	nanoseconds timeD(timeDIF);

	data->type = DTYPE_DATA;
	int current_burst = 0;
	int current_burst_size = MIN_BURST + rand() % DIF_BURST;

	auto t = system_clock::now() + nanoseconds(current_burst_size * nsPDU);
	while (remaining > 0) {
		data->size = MIN_PDU + rand() % DIF_PDU;
		data->seqId++;
		data->ping_time = duration_cast<milliseconds>(system_clock::now().time_since_epoch() - timeD).count();

		if (write(fd, buffer, data->size) != data->size) {
			LOG_ERR("FAILED AT SENDING DATA MESSAGE - ABORT FLOW");
			return false;
		}

		remaining--;
		current_burst++;

		if (current_burst >= current_burst_size) {
			if (busyWait) {
				while (system_clock::now() < t) {}
			} else {
				if (t > system_clock::now()) {
					sleep_until(t);
				}
			}
		}
		current_burst = 0;
		current_burst_size = MIN_BURST + rand() % DIF_BURST;
		t += nanoseconds(current_burst_size * nsPDU);
	}

	data->size = sizeof(dataSDU);
	data->type = DTYPE_FIN;
	if (write(fd, buffer, data->size) != data->size) {
		LOG_ERR("FAILED AT SENDING FIN MESSAGE - ABORT FLOW");
		return false;
	}

	return true;
}
