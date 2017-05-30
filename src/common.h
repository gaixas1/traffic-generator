#pragma once
#include <chrono>
#ifndef RINA_PREFIX
	#define RINA_PREFIX "COMMON"
#endif // !RINA_PREFIX
#include <librina/librina.h>
#include <librina/logs.h>

using namespace std;
using namespace std::chrono;
using namespace rina;

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