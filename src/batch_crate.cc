#include "batch_crate.h"
#include <fstream>
#include <thread>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#ifndef RINA_PREFIX
#define RINA_PREFIX "batch_crate"
#endif // !RINA_PREFIX

#include <librina/librina.h>
#include <librina/logs.h>
#include "bcommon.h"

using namespace std;
using namespace std::chrono;
using namespace std::this_thread;
using namespace rina;

void batch_crate::setPDU(int B) {
	if (B < (int)sizeof(SDU) ) {
		throw std::invalid_argument("received non-positive or \"< headers size\" value for PDU size");
	}
	PDU_S = B;
}
void batch_crate::setDuration(int s) {
	duration = s;
}

	
void batch_crate::handle_flow(int port_id, int fd) {
	srand (time(NULL));
	union {
		char buffer[BUFF_SIZE];
		SDU data;
	};
	
	data.flowIdent = flowIdent;
	system_clock::time_point now = system_clock::now();
	
	
	char * bdata = buffer + sizeof(SDU);
	string appIdent = name+"-"+instance;
	int plen = sizeof(SDU) + appIdent.length() + 1;
	if(plen > BUFF_SIZE) {
		plen = BUFF_SIZE;
	}
	memcpy(bdata, appIdent.c_str(), plen-sizeof(SDU));
	data.len = plen;
	data.t = duration_cast<milliseconds>(now.time_since_epoch()).count();
	if (write(fd, buffer, plen) != plen) {	
		cout << "First PDU error!!!"<<endl;
		return;
	}
	
	* ((int*)bdata) = n;
	data.len = sizeof(SDU) + sizeof(int);
	data.t = duration_cast<milliseconds>(now.time_since_epoch()).count();
	if (write(fd, buffer, plen) != plen) {	
		cout << "Second PDU error!!!"<<endl;
		return;
	}
	
	
	data.len = PDU_S;
	
	double dinterval = 8000000.0 * PDU_S / bps;
	microseconds interval((int)dinterval);
	now = system_clock::now();
	
	for(int i = 0; i < n; i++) {
		_flow f;
		f.id = i;
		f.next_sq = 0;
		f.next_t = now + microseconds(rand() % (int)dinterval);
		flows.push_back(f);
	}
	
	system_clock::time_point end_t = now;
	if(duration <= 0) {
		end_t += minutes(1);
	} else {
		end_t += seconds(duration);
	}
	
	system_clock::time_point next_t = end_t;
	for(int i = 0; i < n; i++) {
		if(next_t > flows[i].next_t){
			next_t = flows[i].next_t;
		}
	}
	
	try {
		
		if (busyWait) {
			while(system_clock::now() < next_t) {}
		} else if (system_clock::now() < next_t) {
			sleep_until(next_t);
		}
		
		int k = 0;
		while(next_t < end_t) {
			next_t = end_t;
			now = system_clock::now();
			for(int i = 0; i < n; i++) {
				_flow & f = flows[i];
				
				if(f.next_t <= now) {
					//SEND PDU
					data.id = f.id;
					data.sq = f.next_sq++;
					data.t = duration_cast<milliseconds>(now.time_since_epoch()).count();
					if (write(fd, buffer, data.len) != data.len) {
						try{
							release_flow(port_id);
						} catch(...){}
						return;
					}
					k++;
					
					//NEXT PDU
					f.next_t = now + interval;
					
					if(next_t > f.next_t){
						next_t = f.next_t;
					}
				}
			}
			
			if (busyWait) {
				while(system_clock::now() < next_t) {}
			} else if (system_clock::now() < next_t) {
				sleep_until(next_t);
			}
		}
		
	} catch (...) {}
	
	try{
		release_flow(port_id);
	} catch(...){}
}
