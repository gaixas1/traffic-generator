#include "chat_common.h"
#include <string>

using namespace std;


baseMsg::baseMsg(int _type, int _size) :
	type(_type), size(_size) {}

bool read_data(int fd, char * buffer) {
	baseMsg * data = (baseMsg*)&buffer;
	data->size = 2*sizeof(int);
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


string getParameter(string m) {
	return "";
}

string getSecondParameter(string m) {
	return "";
}