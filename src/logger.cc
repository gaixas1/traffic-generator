#include "logger.h"

#ifndef RINA_PREFIX
#define RINA_PREFIX "LOGGER"
#endif // !RINA_PREFIX

#include <librina/librina.h>
#include <librina/logs.h>
#include <iostream>
#include <vector>
#include <fstream>

#include "bcommon.h"

using namespace std;
using namespace std::chrono;
using namespace rina;

void logger::handle_flow(int port_id, int fd) {
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
	string clientName = bdata;
	
	cout << "Flow " << port_id << ","<< fd <<" name : "<< clientName << endl;
	long long t = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
	cout << "Flow " << port_id << ","<< fd <<" base latency : "<< (t-data.t) << " ms" << endl;

	
	if(!read_SDU(fd, buffer)){
		cout << "Flow " << port_id << ","<< fd <<" error second SDU" << endl;
		return;
	}
	
	long long init_T = data.t;
	
	int n = *(int*)bdata;
	cout << "Flow " << port_id << ","<< fd <<" number of sub-flows "<< n << endl;
	
	vector<fstream  *> files;
	try {
		for(int i = 0; i < n; i++) {
			string filename = "./logs/"+name+"-"+instance+"-"+clientName+"-"+to_string(i)+".csv";
			cout << filename<< endl;
			
			fstream  * f = new fstream ();
			f->open(filename.c_str(),fstream::out | fstream::trunc);
			files.push_back(f);
			*f << 
				"SEQ" 
				<< ";" << "T" 
				<< ";" << "Bytes"
				<< ";" << "DT" <<endl;
				
		}
	} catch(...){
		cout << "Error creating log files!!!"<<endl;
		return;
	}
	
	int count = 0;
	long long len = 0;
	try {
		for(;;){
			if(!read_SDU(fd, buffer)) {
				break;
			}
			count++;
			len += data.len;
			long long dt = data.t - init_T;
			t = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
			*(files[data.id]) << data.sq  << ";" << dt << ";" << data.len << ";" << (t-data.t) <<endl;
		}
	} catch(...){}
	
	
	
	cout << "Flow " << port_id << ","<< fd <<" end" << endl;
	cout << "DATA:: " << count << ","<< len << endl;
	try{
		for(fstream * f : files) {
			f->close();
		}
		release_flow(port_id);
	} catch(...){}
}