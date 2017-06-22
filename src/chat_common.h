#pragma once

#include <string>

#define MAX_BUFFER 5000

#define DTYPE_MSG 1
#define DTYPE_PV 2

#define DTYPE_NICK 3
#define DTYPE_CLOSE 4
#define DTYPE_RGFILE 5
#define DTYPE_RQFILE 6
#define DTYPE_LFILES 7
#define DTYPE_LUSERS 8

using namespace std;

struct baseMsg {
	int type;		//DTYPE_X
	int size;		//in Bytes
	baseMsg(int _type, int _size);
};

bool read_data(int fd, char * buffer);

string getParameter(string m);
string getSecondParameter(string m);