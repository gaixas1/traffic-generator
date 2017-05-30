#include "common.h"


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
