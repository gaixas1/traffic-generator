#include "voice_client.h"
#include <fstream>
#include <thread>
#include <stdlib.h>
#include <time.h>

#ifndef RINA_PREFIX
#define RINA_PREFIX "VOICE_CLIENT"
#endif // !RINA_PREFIX
#include "common.h"

using namespace std;
using namespace std::chrono;
using namespace std::this_thread;
using namespace rina;


void voice_client::handle_flow(int port_id, int fd) {
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



bool voice_client::flow(int fd, char * buffer) {
	dataSDU * data = (dataSDU*)buffer;
	int remaining = (MIN_DATA * 2000) / (2 * MIN_PDU + DIF_PDU);

	nanoseconds timeD(timeDIF);

	data->type = DTYPE_DATA;

	sleep_for(milliseconds(rand() % MIN_OFF));

	auto tend = system_clock::now() + seconds(duration);
	do {
		int remaining = (MIN_ON + rand() % DIF_ON) * 1000 / nsPDU;
		auto t = system_clock::now() + timeD;

		while (remaining > 0) {
			data->size = MIN_PDU + rand() % DIF_PDU;
			data->seqId++;
			data->ping_time = duration_cast<milliseconds>(system_clock::now().time_since_epoch() - timeD).count();

			if (write(fd, buffer, data->size) != data->size) {
				LOG_ERR("FAILED AT SENDING DATA MESSAGE - ABORT FLOW");
				return false;
			}

			remaining--;

			if (busy_wait) {
				while(system_clock::now() < t) {}
			} else {
				if (t > system_clock::now()) {
					sleep_until(t);
				}
			}
			t += timeD;
		}
		sleep_for(milliseconds(MIN_OFF + rand() % DIF_OFF));
	} while (system_clock::now() <= tend);
}
