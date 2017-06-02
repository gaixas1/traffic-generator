#include "chat_common.h"

bool read_data(int fd, char * buffer) {
	baseMsg * data = (baseMsg*)&buffer;
	data->size = sizeof(int);
	int padding = 0;
	int ret;

	while (padding < data->size) {
		ret = read(fd, buffer, data->size - padding);
		if (ret < 0) {
			LOG_ERR("read() failed: %s", strerror(errno));
			return false;
		}
		padding += ret;
	}
	return true;
}