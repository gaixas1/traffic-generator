#include "videologger.h"

#ifndef RINA_PREFIX
#define RINA_PREFIX "VIDEOLOGGER"
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

void videologger::handle_flow(int port_id, int fd) {
	union {
		char buffer[BUFF_SIZE];
		SDU data;
	};
	char * bdata = buffer + sizeof(SDU);
	
	int msXpdu = (int) (1000/playHz);
	
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
	
	int n = *(int*)bdata;
	cout << "Flow " << port_id << ","<< fd <<" number of sub-flows "<< n << endl;
	
	vector<fstream  *> files;
	vector<int> counts;
	vector<long long>times;
	vector<bool> stats;
	try {
		for(int i = 0; i < n; i++) {
			string filename = "./logs/"+name+"-"+instance+"-"+clientName+"-"+to_string(i)+".csv";
			cout << filename<< endl;
			
			fstream  * f = new fstream ();
			f->open(filename.c_str(),fstream::out | fstream::trunc);
			files.push_back(f);
			counts.push_back(0);
			times.push_back(0);
			stats.push_back(false);
			*f 
					<< "SEQ"
					<< ";" << "STAT" 
					<< ";" << "STATID"
					<< ";" << "T" <<endl;
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
			long long & t0 = times[data.id];
			count++;
			if(stats[data.id]) {
				long long t1 = NOWCOUNT;
				long long dt = t1-t0;
				
				int c = floor(dt/msXpdu);
				if(c > 0) {
					t0 += c*msXpdu;
					count -= c;
					if (count <= 0) {
						stats[data.id] = false;
						count = 0;
						*files[data.id] << data.sq <<";stop;1;"<< t1 <<endl; 
					}
				} else if(count > maxBuffer) {
					count = maxBuffer;
				}
			} else if(count == minBuffer) {
				stats[data.id] = true;
				t0 = NOWCOUNT;
				*files[data.id] << data.sq <<";start;0;"<< t0 <<endl; 
			}
		}
	} catch(...){}
	
	cout << "Flow " << port_id << ","<< fd <<" end" << endl;
	try{
		for(int i = 0; i < n; i++) {
			fstream * f = files[i];
			*f << "end;2;"<< NOWCOUNT <<endl; 
			f->close();
		}
		release_flow(port_id);
	} catch(...){}
}