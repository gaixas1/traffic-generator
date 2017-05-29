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

void data_client::handle_flow(int port_id, int fd) {
	char buffer[BUFF_SIZE];
	dataSDU * data = (dataSDU*)buffer;
	srand(time(0));

	data->size = sizeof(dataSDU);
	data->type = DTYPE_INIT;
	data->flowIdent = flowIdent;
	data->seqId = 0;
	data->echo = doEcho;
	data->record = doRecord;


	if (!read_data(fd, buffer)) {
		LOG_ERR("FIRST READ FAILED - ABORT FLOW");
		release_flow(port_id);
		return;
	}

	if (data->type == DTYPE_INIT) {
		LOG_ERR("WRONG INITIAL MESSAGE - ABORT FLOW");

		release_flow(port_id);
		return;
	}

	thread * echo_t = nullptr;
	if (doEcho) {
		echo_t = new thread(echo_listener, fd, flowIdent);
	}

	bool result = flow(fd, buffer);

	if (doEcho) {
		echo_t->join();
		delete echo_t;
	}

	if (!result) {
		LOG_ERR("SOMETHING FAILED - ABORT FLOW");
	}

	release_flow(port_id);
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
			if (busy_wait) {
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
