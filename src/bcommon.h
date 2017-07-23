#pragma once
#include <string>
#include <chrono>

using namespace std;
using namespace std::chrono;

#define BUFF_SIZE 1500
#define NOW system_clock::now()

struct SDU {
	int len;
	short id;
	short sq;
	long long t;
};

bool read_SDU(int fd, char * buffer);
bool sendInit(int fd, char * buffer, string name, string instance, int n);