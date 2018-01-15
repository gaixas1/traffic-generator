#pragma once
#include <chrono>
#include <fstream>

using namespace std;
using namespace std::chrono;

#define BUFF_SIZE 1500

#define DTYPE_INIT	1
#define DTYPE_DATA	2
#define DTYPE_FIN	3

struct dataSDU {
	unsigned int size;		//in Bytes
	int type;				//DTYPE_X
	int seqId;
	long long send_time;
};

bool read_dataSDU(int fd, char * buffer);

bool timed_read_dataSDU(int fd, char * buffer, int sec, int usec);

struct initSDU { //DTYPE_INIT
	int flowId;
	int QoSId;
};