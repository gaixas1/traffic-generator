#include "test_common.h"
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
			if (ret == 0) {
				LOG_ERR("read() failed: return 0");
			}
			else {
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
			if (ret == 0) {
				LOG_ERR("read() failed: return 0");
			}
			else {
				LOG_ERR("read() failed: %s", strerror(errno));
			}
			return false;
		}
		buffer += ret;
		rem -= ret;
	} while (rem > 0);
	return true;
}

bool timed_read_dataSDU(int fd, char * buffer, int sec, int usec) {
	fd_set read_fds, write_fds, except_fds;
	FD_ZERO(&read_fds);
	FD_ZERO(&write_fds);
	FD_ZERO(&except_fds);
	FD_SET(fd, &read_fds);

	// Set timeout to 1.0 seconds
	struct timeval timeout;
	timeout.tv_sec = sec;
	timeout.tv_usec = usec;

	if (select(fd + 1, &read_fds, &write_fds, &except_fds, &timeout) != 1) {
		return false;
	}
	return read_dataSDU(fd, buffer);
}