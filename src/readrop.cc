#include "readrop.h"

#ifndef RINA_PREFIX
#define RINA_PREFIX "READROP"
#endif // !RINA_PREFIX

#include <librina/librina.h>
#include <librina/logs.h>
#include <iostream>

#include "bcommon.h"

using namespace std;
using namespace rina;

void readrop::handle_flow(int port_id, int fd) {
	union {
		char buffer[BUFF_SIZE];
		SDU data;
	};
	char * bdata = buffer + sizeof(SDU);
	
	cout << "Flow " << port_id << ","<< fd <<" start" << endl;
	
	
	if(!read_SDU(fd, buffer)){
		cout << "Flow " << port_id << ","<< fd <<" error first SDU" << endl;
		return;
	}
	
	cout << "Flow " << port_id << ","<< fd <<" name : "<< bdata << endl;
	
	
	int count = 0;
	long long len = 0;
	try {
		int ret = 1;
		for(;ret > 0;){
			ret = read(fd, buffer, BUFF_SIZE);
			if(ret > 0) {
				len+=ret;
				count++;
			}
		}
	} catch(...){}
	cout << "Flow " << port_id << ","<< fd <<" end" << endl;
	cout << "DATA:: " << count << ","<< len << endl;
	try{
		release_flow(port_id);
	} catch(...){}
}