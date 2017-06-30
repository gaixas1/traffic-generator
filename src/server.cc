#include "server.h"
#include <fstream>
#include <time.h>


#ifndef RINA_PREFIX
#define RINA_PREFIX "SERVER"
#endif // !RINA_PREFIX

#include <librina/librina.h>
#include <librina/logs.h>
#include "common.h"
#include <iostream>

using namespace std;
using namespace std::chrono;
using namespace rina;

void server::setRecordInterval(bool stat, int ms) {
	print_interval = stat;
	stats_interval = ms;
}

void server::setRecordRange(int ms) {
	rangeLat = ms;
}

void server::handle_flow(int port_id, int fd) {
	char buffer[BUFF_SIZE];
	dataSDU * data = (dataSDU*)buffer;
	try{
		if (!read_dataSDU(fd, buffer) || data->type != DTYPE_INIT) {
			LOG_ERR("FIRST READ FAILED - ABORT FLOW");
			release_flow(port_id);
			return;
		}
		data->type = DTYPE_START;
		data->size = sizeof(dataSDU);
		data->pong_time = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
		if (write(fd, buffer, sizeof(dataSDU)) != sizeof(dataSDU)) {
			LOG_ERR("FAILED AT SENDING START MESSAGE - ABORT FLOW");
			release_flow(port_id);
			return;
		}
	} catch(...){
		LOG_ERR("EXCEPTION CATCHED - Something failed");
		return;
	}
	
	ofstream log, logrecord;
	string filename = "SERVER_"+name + "_" + instance + "_" + to_string(data->flowIdent);
	log.open(filename);
	log << "Initial Time = " << (data->pong_time - data->ping_time) << endl;
	log << "TYPE;Index;Time-0;Time-1;Duration;PDUs;Data;PDUs\\s;bps;Success prob.;Min Lat.;Avg Lat.;Max Lat.;Std.Dev.Lat" << endl;	
	if (data->record > 0) {
		filename = name + "_" + instance + "_record_" + to_string(data->flowIdent);
		logrecord.open(filename);
	}

	bool result = flow(fd, buffer, log, data->echo != 0, data->record > 0, logrecord);

	if (log.is_open()) { log.close(); }
	if (logrecord.is_open()) { logrecord.close(); }

	if (!result) {
		LOG_ERR("SOMETHING FAILED");
	}
}

bool server::flow(int fd, char * buffer, ofstream & log, bool echo, bool record, ofstream & recordlog) {
	dataSDU * data = (dataSDU*)buffer;
	auto interval = milliseconds(stats_interval);

	long countCurrent = 1;
	int currentSeq = data->seqId;
	long long latCurrent = data->pong_time - data->ping_time;


	stats full_stats(high_resolution_clock::now(), currentSeq);
	stats partial_stats(high_resolution_clock::now(), currentSeq);
	int i = 0;

	try {
		while(data->type != DTYPE_FIN) {
			if (!read_dataSDU(fd, buffer)) { return false; }
		
			if (data->type != DTYPE_DATA && data->type != DTYPE_FIN) {
				LOG_ERR("RECEIVED INVALID MESSAGE");
				switch(data->type) {
					case DTYPE_INIT : LOG_ERR("DTYPE_INIT"); break;
					case DTYPE_START : LOG_ERR("DTYPE_START"); break;
					default : LOG_ERR("???"); cerr <<  "Type "<< data->type<<endl;
				}
				return false;
			}
			
			data->pong_time = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
			if ((data->type == DTYPE_FIN || echo) && write(fd, buffer, data->size) != data->size) {
				LOG_ERR("FAILED AT SENDING DATA MESSAGE - ABORT FLOW");
				return false;
			}
			
			long long lat = data->pong_time - data->ping_time;
			full_stats.sample(data->size, lat);
				
			if (record) {
				long long L = lat / rangeLat;
				if (currentSeq >= data->seqId) {
					LOG_INFO("RECEIVE DATA SDU UNORDERED - SKIP DATA");
					continue;
				}
				if (currentSeq + 1 != data->seqId) {
					recordlog << latCurrent << ";" << countCurrent << endl;
					recordlog << 0 << ";" << (data->seqId - currentSeq - 1) << endl;
					latCurrent = L;
					countCurrent = 1;
				} else if (L != latCurrent) {
					if (countCurrent > 0) {
						recordlog << latCurrent << ";" << countCurrent << endl;
					}
					latCurrent = L;
					countCurrent = 1;
				} else {
					countCurrent++;
				}
				currentSeq = data->seqId;
			}
			
			if (print_interval) {
				partial_stats.sample(data->size, lat);
				auto t1 = high_resolution_clock::now();
				if (t1 > partial_stats.t0 + interval) {
					log << "Interval;" << i++ << ";";
					partial_stats.print(high_resolution_clock::now(), data->seqId, log);
					partial_stats = stats(t1, data->seqId);
				}
			}
		}
	} catch(...){
		LOG_ERR("EXCEPTION CATCHED - ABORT FLOW");
		return false;
	}
	if (record && countCurrent > 0) {
		recordlog << latCurrent << ";" << countCurrent << endl;
	}

	full_stats.print(high_resolution_clock::now(), currentSeq, log);
	
	return true;
}