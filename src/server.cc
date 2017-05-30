#include "server.h"
#include <fstream>

#ifndef RINA_PREFIX
#define RINA_PREFIX "SERVER"
#endif // !RINA_PREFIX
#include "common.h"

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

	data->type = DTYPE_START;
	data->size = sizeof(dataSDU);
	data->pong_time = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
	if (write(fd, buffer, sizeof(dataSDU)) != sizeof(dataSDU)) {
		LOG_ERR("FAILED AT SENDING START MESSAGE - ABORT FLOW");

		release_flow(port_id);
		return;
	}


	ofstream log, logrecord;
	string filename = "SERVER_"+name + "_" + instance + "_" + to_string(data->flowIdent);
	log.open(filename);
	log << "Initial Latency = " << (data->pong_time - data->ping_time) << endl;

	
	log << "TYPE;Index;Time;Duration;PDUs;Data;PDUs\s;bps;Success prob.;Min Lat.;Min Lat.;Avg Lat.;Max Lat.;Std.Dev.Lat" << endl;
	

	if (data->record != 0) {
		filename = name + "_" + instance + "_record_" + to_string(data->flowIdent);
		logrecord.open(filename);
	}

	bool result = flow(fd, buffer, log, data->echo != 0, data->record != 0, logrecord);

	log.close();
	if (logrecord.is_open()) {
		logrecord.close();
	}

	if (!result) {
		LOG_ERR("SOMETHING FAILED - ABORT FLOW");
	}

	release_flow(port_id);
}

bool server::flow(int fd, char * buffer, ofstream & log, bool echo, bool record, ofstream & recordlog) {
	dataSDU * data = (dataSDU*)&buffer;

	auto interval = milliseconds(stats_interval);

	long long latCurrent = 0;
	long countCurrent = 0;
	int currentSeq = data->seqId;

	stats full_stats(high_resolution_clock::now(), currentSeq);
	stats partial_stats(high_resolution_clock::now(), currentSeq);
	int i = 0;


	for (;;) {
		if (!read_data(fd, buffer)) {
			return false;
		}

		if (data->type == DTYPE_DATA) {
			data->pong_time = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
			
			long long lat = data->pong_time - data->ping_time;
			full_stats.sample(data->size, lat);

			if (record) {
				long long L = lat / rangeLat;

				if (currentSeq >= data->seqId) {
					LOG_INFO("RECEIVE DATA SDU UNORDERED - SKIP DATA");
					continue;
				}
				if (currentSeq + 1 != data->seqId) {
					recordlog << latCurrent << "\t" << countCurrent << endl;
					recordlog << 0 << "\t" << (data->seqId - currentSeq - 1) << endl;
					latCurrent = lat;
					countCurrent = 1;
				} else if (L != latCurrent) {
					if (countCurrent > 0) {
						recordlog << latCurrent << "\t" << countCurrent << endl;
					}
					latCurrent = lat;
					countCurrent = 1;
				}
			}

			currentSeq = data->seqId;

			if (echo) {
				if (write(fd, buffer, data->size) != data->size) {
					LOG_ERR("FAILED AT SENDING DATA MESSAGE - ABORT FLOW");
					return false;
				}
			}

			if (print_interval) {
				partial_stats.sample(data->size, lat);

				auto t1 = high_resolution_clock::now();
				if (t1 < partial_stats.t0 + interval) {
					log << "Interval;" << i++ << ";";
					partial_stats.print(high_resolution_clock::now(), currentSeq, log);
					partial_stats = stats(t1, currentSeq);
				}
			}
		}
		else if (data->type == DTYPE_FIN) {
			break;
		}
		else {
			LOG_ERR("RECEIVED INVALID MESSAGE TYPE");
			return false;
		}
	}

	data->pong_time = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
	if (write(fd, buffer, sizeof(dataSDU)) != sizeof(dataSDU)) {
		LOG_ERR("FAILED AT SENDING FIN MESSAGE - ABORT FLOW");
		return false;
	}

	log << "FIN;" << i << ";";
	full_stats.print(high_resolution_clock::now(), currentSeq, log);

	return true;
}