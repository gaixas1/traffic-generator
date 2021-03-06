#include "common.h"
#include <climits>

#ifndef RINA_PREFIX
#define RINA_PREFIX "COMMON"
#endif // !RINA_PREFIX

#include <librina/librina.h>
#include <librina/logs.h>
#include <iostream>

using namespace std;
using namespace std::chrono;
using namespace rina;

bool read_dataSDU(int fd, char * buffer) {
	dataSDU * data = (dataSDU*)buffer;
	int rem = sizeof(int);
	
	do {
		int ret = read(fd, buffer, rem);
		if (ret <= 0) {
			if(ret == 0) {
				LOG_ERR("read() failed: return 0");
			} else {
				LOG_ERR("read() failed: %s", strerror(errno));
			}
			return false;
		}
		buffer += ret;
		rem -= ret;
	} while (rem > 0);
	rem += data->size - sizeof(int);
	do {
		int ret = read(fd, buffer, rem);
		if (ret <= 0) {
			if(ret == 0) {
				LOG_ERR("read() failed: return 0");
			} else {
				LOG_ERR("read() failed: %s", strerror(errno));
			}
			return false;
		}
		buffer += ret;
		rem -= ret;
	} while (rem > 0);
	return true;
}



stats::stats(time_point<high_resolution_clock> _t0, int _seq0) 
	: t0(_t0), seq0(_seq0), pdu_num(0), pdu_data(0), m(0), S(0), n(0), minLat(LLONG_MAX), maxLat(0) {}

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

void stats::print(time_point<high_resolution_clock> t1, int seq1, ofstream & log) {
	long long duration = duration_cast<milliseconds>(t1 - t0).count();
	double rate_num = (double)pdu_num * 1000.0 / (double)duration;
	double rate_data = (double)pdu_data * 8000.0 / (double)duration;
	int total = seq1 - seq0;
	double received = (double)pdu_num / (double)total;

	log << t0.time_since_epoch().count() 
		<< ";" << t1.time_since_epoch().count() 
		<< ";" << duration
		<< ";" << pdu_num << ";" << pdu_data
		<< ";" << rate_num << ";" << rate_data
		<< ";" << received
		<< ";" << minLat << ";" << m << ";" << maxLat << ";" << (sqrt(S / n))
		<< "\n";
}