#include "server.h"
#include <string>
#include <fstream>

#ifndef RINA_PREFIX
#define RINA_PREFIX "SERVER"
#endif // !RINA_PREFIX
#include "common.h"

using namespace std;
using namespace std::chrono;
using namespace rina;

void server::handle_flow(int port_id, int fd) {
	char buffer[BUFF_SIZE];
	dataSDU * data = (dataSDU*) &buffer;

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
	string filename = name + "_" + instance + "_" + to_string(data->flowIdent);
	log.open(filename);
	log << "Initial Latency = " << (data->pong_time - data->ping_time) << endl;


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

struct stats {
	time_point<steady_clock> t0;
	long pdu_num;
	long long pdu_data;
	double m, S, n;
	long long minLat, maxLat;

	stats(time_point<steady_clock> _t0) : t0(_t0), pdu_num(0), pdu_data(0), m(0), S(0), n(0), minLat(LLONG_MAX), maxLat(LLONG_MIN) {}
	void sample(int len, long long lat) {
		pdu_num++;
		pdu_data += len;
		if (minLat > lat) { minLat = lat; }
		if (maxLat < lat) { maxLat = lat; }
		double m0 = m;
		n = n + 1;
		m = m + (lat - m) / n;
		S = S + (lat - m)*(lat - m0);
	}
	void print(time_point<steady_clock> t1, ofstream & log) {
		long long duration = duration_cast<milliseconds>( t1 - t0).count();
		double rate_num = (double)pdu_num * 1000.0 / (double)duration;
		double rate_data = (double)pdu_data * 8000.0 / (double)duration;
		log << t1.time_since_epoch << "\tDuration" << duration << " ms"
			<< "\tPDUs" << pdu_num << "(" << pdu_data << " B)"
			<< "\tPDUs" << rate_num << " PDU/s (" << rate_data << " bps)"
			<< "\tLatency" << minLat << " / " << m << " / " << maxLat << " ms | Std dev " <<(sqrt(S / n))
			<< endl;
	}
};


bool server::flow(int fd, char * buffer, ofstream & log, bool echo, bool record, ofstream & recordlog) {
	dataSDU * data = (dataSDU*)&buffer;

	auto interval = milliseconds(stats_interval);

	stats full_stats(high_resolution_clock::now());
	stats partial_stats(high_resolution_clock::now());
	int i = 0;

	long long latCurrent = 0;
	long countCurrent = 0;
	int currentSeq = data->seqId;
	rangeLat = 10;

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
				} else if (currentSeq + 1 != data->seqId) {
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
					log << "Interval " << i++ << " - ";
					full_stats.print(high_resolution_clock::now(), log);
					partial_stats = stats(t1);
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

	log << "Flow ended - ";
	full_stats.print(high_resolution_clock::now(), log);

	return true;
}