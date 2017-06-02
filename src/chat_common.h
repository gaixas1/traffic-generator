#pragma once

struct baseMsg {
	int size;		//in Bytes
	int type;		//DTYPE_X
};

bool read_data(int fd, char * buffer);