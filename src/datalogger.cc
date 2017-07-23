#include "datalogger.h"

#ifndef RINA_PREFIX
#define RINA_PREFIX "DATALOGGER"
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

void datalogger::handle_flow(int port_id, int fd) {
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
	
	long long init_T = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
	
	int n = *(int*)bdata;
	cout << "Flow " << port_id << ","<< fd <<" number of sub-flows "<< n << endl;
	
	vector<fstream  *> files;
	vector<int> counts;
	vector<long long> lenghts;
	try {
		for(int i = 0; i < n; i++) {
			string filename = "./logs/"+name+"-"+instance+"-"+clientName+"-"+to_string(i)+".csv";
			cout << filename<< endl;
			
			fstream  * f = new fstream ();
			f->open(filename.c_str(),fstream::out | fstream::trunc);
			files.push_back(f);
			counts.push_back(0);
			lenghts.push_back(0);
			*f 
					<< "SEQ"
					<< ";" << "COUNT" 
					<< ";" << "Bytes"
					<< ";" << "DT" <<endl;
		}
	} catch(...){
		cout << "Error creating log files!!!"<<endl;
		return;
	}
	
	try {
		for(;;){
			if(!read_SDU(fd, buffer)) {
				break;
			}
			int & count = counts[data.id];
			long long & lenght = lenghts[data.id];
			count++;
			lenght+=data.len;
			t = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
			if(count >= countToLog) {
				*files[data.id] 
					<< data.sq  
					<< ";" << count 
					<< ";" << lenght
					<< ";" << (t-init_T) <<endl;
				count = 0;
				lenght = 0;
			}
		}
	} catch(...){}
	
	
	
	cout << "Flow " << port_id << ","<< fd <<" end" << endl;
	try{
		for(int i = 0; i < n; i++) {
			fstream * f = files[i];
			int & count = counts[data.id];
			long long & lenght = lenghts[data.id];
			t = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
			*f 
					<< data.sq  
					<< ";" << count 
					<< ";" << lenght
					<< ";" << (t-init_T) <<endl;
			f->close();
		}
		release_flow(port_id);
	} catch(...){}
}