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
	int rem = sizeof(int);
	do {
		cout << "TR " << rem << "/" <<  (void*)buffer <<endl;
		int ret = read(fd, buffer, rem);
		cout << "RR " << ret <<endl;
		if (ret <= 0) { return false; }
		buffer += ret;
		rem -= ret;
		cout << "QR " << rem << "/" << (void*)buffer <<endl;
	} while (rem > 0);
	rem += data->len - sizeof(int);
	cout << "TRem " << rem <<endl;
	cout << (int)buffer[0] << "."<< (int)buffer[1] << "."<< (int)buffer[2] << "."<< (int)buffer[3] << "."<< endl;
	do {
		cout << "TR " << rem << "/" << (void*)buffer <<endl;
		int ret = read(fd, buffer, rem);
		cout << "RR " << ret <<endl;
		if (ret <= 0) {
			return false;
		}
		buffer += ret;
		rem -= ret;
		cout << "QR " << rem << "/" << (void*)buffer <<endl;
	} while (rem > 0);
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
	if (write(fd, buffer, data->len) != data->len) {	 
		cout << "First PDU error!!!"<<endl;
		return false;
	}
	cout << "S : " << data->len << " / "<< data->id << " / "<< data->sq<< " / "<< data->t <<endl;
	
	//Send initial 
	* ((int*)bdata) = n;
	data->len = sizeof(SDU) + sizeof(int);
	data->t = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
	if (write(fd, buffer, data->len) != data->len) {	
		cout << "Second PDU error!!!"<<endl;
		return false;
	}
	cout << "S : " << data->len << " / "<< data->id << " / "<< data->sq<< " / "<< data->t <<endl;
	
	return true;
}