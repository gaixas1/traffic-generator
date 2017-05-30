#pragma once
#include <chrono>
#include <fstream>

using namespace std;
using namespace std::chrono;

#define BUFF_SIZE 1500

#define DTYPE_INIT 1
#define DTYPE_DATA 2
#define DTYPE_FIN 3
#define DTYPE_START 4

struct dataSDU {
	int size;		//in Bytes
	int type;		//DTYPE_X
	int flowIdent;	//Unique
	int seqId;
	char echo;		//true != 0
	char record;	//true != 0
	long long ping_time;
	long long pong_time;
};

bool read_data(int fd, char * buffer);

struct stats {
	time_point<steady_clock> t0;
	int seq0;
	long pdu_num;
	long long pdu_data;
	double m, S, n;
	long long minLat, maxLat;

	stats(time_point<steady_clock> _t0, int _seq0);
	void sample(int len, long long lat);
	void print(time_point<steady_clock> t1, int seq1, ofstream & log);
};