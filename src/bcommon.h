#pragma once
#include <string>
#include <chrono>

using namespace std;
using namespace std::chrono;

#define BUFF_SIZE 1500
#define MIN_BUFF_SIZE 32
#define NOW system_clock::now()
#define NOWCOUNT duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count()

struct SDU {
	int32_t len;
	int16_t id;
	int16_t sq;
	int64_t t;
};

bool read_SDU(int fd, char * buffer);
bool sendInit(int fd, char * buffer, string name, string instance, int n);