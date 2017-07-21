#pragma once
#include <chrono>
#include <fstream>
#include <iostream>

using namespace std;
using namespace std::chrono;

#define BUFF_SIZE 1500

struct SDU {
	int len;
	int flowIdent;
	int id;
	unsigned long long sq;
	long long t;
};

bool read_SDU(int fd, char * buffer) {
	SDU * data = (SDU*)buffer;
	int rem = sizeof(int);
	do {
		int ret = read(fd, buffer, rem);
		if (ret <= 0) { return false; }
		buffer += ret;
		rem -= ret;
	} while (rem > 0);
	rem += data->len - sizeof(int);
	do {
		int ret = read(fd, buffer, rem);
		if (ret <= 0) {
			return false;
		}
		buffer += ret;
		rem -= ret;
	} while (rem > 0);
	return true;
}