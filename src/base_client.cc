#include "base_client.h"

#include <fstream>
#include <thread>
#include <stdlib.h>
#include <time.h>
#include <stdexcept>


#ifndef RINA_PREFIX
#define RINA_PREFIX "DATA_CLIENT"
#endif // !RINA_PREFIX

#include <librina/librina.h>
#include <librina/logs.h>

using namespace std;
using namespace std::chrono;
using namespace std::this_thread;
using namespace rina;

void base_client::setPDUSize(int min, int max) {
	if(min < (int)sizeof(dataSDU)) { min = sizeof(dataSDU); }
	if(max < (int)sizeof(dataSDU)) { max = sizeof(dataSDU); }
	
	if (min == max) {
		MIN_PDU = min;
		DIF_PDU = 0;
	} else if (min < max) {
		MIN_PDU = min;
		DIF_PDU = max - min;
	} else {
		MIN_PDU = max;
		DIF_PDU = min - max;
	}
}

void base_client::setInterval(int ns) {
	if(ns <= 0){ nsPDU = 1; }
	else {nsPDU = ns;}
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
	data->ping_time = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
	try {
		if (write(fd, buffer, data->size) != data->size) {
			LOG_ERR("FAILED AT SENDING INIT MESSAGE - ABORT FLOW");
			release_flow(port_id);
			return;
		}
		if (!read_dataSDU(fd, buffer) || data->type != DTYPE_START) {
			LOG_ERR("FIRST READ FAILED - ABORT FLOW");
			release_flow(port_id);
			return;
		}
	} catch(...){
		LOG_ERR("EXCEPTION CATCHED - Something failed");
		return;
	}
	
	thread * echo_t = nullptr;
	if (doEcho) {
		string filename = "CLIENT_" + name + "_" + instance + "_" + to_string(data->flowIdent);
		echo_t = new thread(&echo_listener, fd, filename, print_interval, stats_interval);
	}

	bool result = flow(fd, buffer);
	
	if(result) {
		data->type = DTYPE_FIN;
		data->size = sizeof(data);
		try {		
			if (write(fd, buffer, data->size) != data->size) {
				LOG_ERR("FAILED AT SENDING INIT MESSAGE - ABORT FLOW");
				return;
			}	
		} catch(...){
			LOG_ERR("EXCEPTION CATCHED - Something failed");
			return;
		}
	}

	if (doEcho && echo_t != nullptr) {
		echo_t->join();
		delete echo_t;
	} else {
		result &= fin_listener(fd);
	}

	if (!result) {
		LOG_ERR("SOMETHING FAILED");
	}
	release_flow(port_id);
}

bool echo_listener(int fd, string filename, bool interval_stats, int interval_ms) {
	char buffer[BUFF_SIZE];
	dataSDU * data = (dataSDU*)buffer;

	ofstream log;
	log.open(filename);
	log << "TYPE;Index;Time-0;Time-1;Duration;PDUs;Data;PDUs\\s;bps;Success prob.;Min RTT.;Avg RTT.;Max RTT.;Std.Dev.RTT" << endl;

	auto interval = milliseconds(interval_ms);
	int currentSeq = 0;
	stats full_stats(high_resolution_clock::now(), currentSeq);
	stats partial_stats(high_resolution_clock::now(), currentSeq);
	int i = 0;

	while (data->type != DTYPE_FIN) {
		try{
			if (!read_dataSDU(fd, buffer)) { return false; }
		} catch(...){
			LOG_ERR("EXCEPTION CATCHED - Read failure");
			return false;
		}
		
		if (data->type != DTYPE_DATA && data->type != DTYPE_FIN) {
			LOG_ERR("RECEIVED INVALID MESSAGE TYPE");
			return false;
		}
		
		long long rtt = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count() - data->ping_time;
		full_stats.sample(data->size, rtt);
		currentSeq = data->seqId;
		if (interval_stats) {
			partial_stats.sample(data->size, rtt);
			auto t1 = high_resolution_clock::now();
			if (t1 > partial_stats.t0 + interval) {
				log << "Interval;" << i++ << ";";
				partial_stats.print(high_resolution_clock::now(), currentSeq, log);
				partial_stats = stats(t1, currentSeq);
			}
		}
	}

	log << "FIN;" << i << ";";
	full_stats.print(high_resolution_clock::now(), currentSeq, log);
	log.close();
	return true;
}

bool fin_listener(int fd) {
	char buffer[BUFF_SIZE];
	dataSDU * data = (dataSDU*)buffer;

	try{
		if (!read_dataSDU(fd, buffer)) { return false; }
	} catch(...){
		LOG_ERR("EXCEPTION CATCHED - Read failure");
		return false;
	}
		
	if (data->type == DTYPE_FIN) {
		return true;
	} else {
		LOG_ERR("RECEIVED INVALID MESSAGE TYPE");
		return false;
	}
}