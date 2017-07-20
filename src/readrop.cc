#include "readrop.h"

#ifndef RINA_PREFIX
#define RINA_PREFIX "READROP"
#endif // !RINA_PREFIX

#include <librina/librina.h>
#include <librina/logs.h>
#include <iostream>

#define BUFF_SIZE 3000

using namespace std;
using namespace rina;

void readrop::handle_flow(int port_id, int fd) {
	char buffer[BUFF_SIZE];
	cout << "Flow " << port_id << ","<< fd <<" start" << endl;
	try {
		int ret = 1;
		for(;ret > 0;){
			ret = read(fd, buffer, BUFF_SIZE);
		}
	} catch(...){}
	cout << "Flow " << port_id << ","<< fd <<" end" << endl;
	try{
		release_flow(port_id);
	} catch(...){}
}