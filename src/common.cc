#include "common.h"

using namespace std;
using namespace std::chrono;

bool read_data(int fd, char * buffer) {
	dataSDU * data = (dataSDU*)&buffer;
	int padding = 0;
	do {
		ret = read(fd, buffer, sizeof(int)-padding);
		if (ret < 0) {
			LOG_ERR("read() failed: %s", strerror(errno));
			return false;
		}
		padding += ret;
	} while (padding < sizeof(int));
	do {
		ret = read(fd, buffer, data->size - padding);
		if (ret < 0) {
			LOG_ERR("read() failed: %s", strerror(errno));
			return false;
		}
		padding += ret;
	} while (padding < data->size);
	return true;
}



stats::stats(time_point<steady_clock> _t0, int _seq0) 
	: t0(_t0), seq0(_seq0), pdu_num(0), pdu_data(0), m(0), S(0), n(0), minLat(LLONG_MAX), maxLat(LLONG_MIN) {}

void stats::sample(int len, long long lat) {
	pdu_num++;
	pdu_data += len;
	if (minLat > lat) { minLat = lat; }
	if (maxLat < lat) { maxLat = lat; }
	double m0 = m;
	n = n + 1;
	m = m + (lat - m) / n;
	S = S + (lat - m)*(lat - m0);
}

void stats::print(time_point<steady_clock> t1, int seq1, ofstream & log) {
	long long duration = duration_cast<milliseconds>(t1 - t0).count();
	double rate_num = (double)pdu_num * 1000.0 / (double)duration;
	double rate_data = (double)pdu_data * 8000.0 / (double)duration;
	int total = seq1 - seq0;
	double received = (double)pdu_num / (double)total;

	log << t1.time_since_epoch << ";" << duration
		<< ";" << pdu_num << ";" << pdu_data
		<< ";" << rate_num << ";" << rate_data
		<< ";" << received
		<< ";" << minLat << ";" << m << ";" << maxLat << ";" << (sqrt(S / n))
		<< "\n";
}