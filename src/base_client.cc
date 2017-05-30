#include "base_client.h"

#include <fstream>
#include <thread>
#include <stdlib.h>
#include <time.h>
#include <stdexcept>

#ifndef RINA_PREFIX
#define RINA_PREFIX "DATA_CLIENT"
#endif // !RINA_PREFIX
#include "common.h"

using namespace std;
using namespace std::chrono;
using namespace std::this_thread;
using namespace rina;

void base_client::setPDUSize(int min, int max) {
	if (min == max) {
		MIN_PDU = min;
		DIF_PDU = 0;
	}
	else if (min < max) {
		MIN_PDU = min;
		DIF_PDU = max - min;
	}
	else {
		MIN_PDU = max;
		DIF_PDU = min - max;
	}
	if (MIN_PDU <= 0) {
		throw std::invalid_argument("received negative value for PDU size");
	}
}

void base_client::setInterval(int ns) {
	nsPDU = ns;
}
void base_client::setRecordInterval(bool stat, int ms) {
	print_interval = stat;
	stats_interval = ms;
}

void base_client::handle_flow(int port_id, int fd) {
	srand(time(0));
	char buffer[BUFF_SIZE];
	dataSDU * data = (dataSDU*)buffer;

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
		echo_t = new thread(&echo_listener, fd, flowIdent, print_interval, stats_interval);
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


bool echo_listener(int fd, int flowIdent, bool interval_stats, int interval_ms) {

	//stats RTT + 
	return true;
}