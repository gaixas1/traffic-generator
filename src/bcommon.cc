#include "bcommon.h"
#include <chrono>
#include <unistd.h>
#include <fcntl.h>
#include <fstream>
#include <string.h>
#include <iostream>

using namespace std;
using namespace std::chrono;

bool read_SDU(int fd, char * buffer) {
	SDU * data = (SDU*)buffer;
	int rem = MIN_BUFF_SIZE;
	do {
		int ret = read(fd, buffer, rem);
		if (ret <= 0) { return false; }
		buffer += ret;
		rem -= ret;
	} while (rem > 0);
	rem += data->len - MIN_BUFF_SIZE;
	while(rem > 0) {
		int ret = read(fd, buffer, rem);
		if (ret <= 0) {
			return false;
		}
		buffer += ret;
		rem -= ret;
	}
	return true;
}

bool sendInit(int fd, char * buffer, string name, string instance, int n){
	SDU * data = (SDU*)buffer;
	char * bdata = buffer + sizeof(SDU);
	
	//Send App information
	string appIdent = name+"-"+instance;
	data->len = sizeof(SDU) + appIdent.length() + 1;
	if(data->len > BUFF_SIZE) {
		data->len = BUFF_SIZE;
	}
	memcpy(bdata, appIdent.c_str(), data->len-sizeof(SDU));
	data->t = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
	if(data->len < MIN_BUFF_SIZE) {
		data->len = MIN_BUFF_SIZE;
	}
	if (write(fd, buffer, data->len) != data->len) {	 
		cout << "First PDU error!!!"<<endl;
		return false;
	}
	cout << "S : " << data->len << " / "<< data->id << " / "<< data->sq<< " / "<< data->t <<endl;
	
	//Send initial 
	* ((int*)bdata) = n;
	data->len = sizeof(SDU) + sizeof(int);
	data->t = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
	if(data->len < MIN_BUFF_SIZE) {
		data->len = MIN_BUFF_SIZE;
	}
	if (write(fd, buffer, data->len) != data->len) {	
		cout << "Second PDU error!!!"<<endl;
		return false;
	}
	cout << "S : " << data->len << " / "<< data->id << " / "<< data->sq<< " / "<< data->t <<endl;
	
	return true;
}