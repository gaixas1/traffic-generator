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
	system_clock::time_point now, end_t, next_t;
	microseconds interval;
	
	if(!sendInit(fd, buffer, name, instance, n)) {
		return;
	}
	
	double dinterval = 8000000.0 * PDU_S / bps;
	interval = microseconds((int)dinterval);
	now = NOW;
	
	data.len = PDU_S;
	
	for(int i = 0; i < n; i++) {
		_flow f(i);
		f.next_t = now + microseconds(rand() % (int)dinterval);
		flows.push_back(f);
	}
	
	next_t = end_t = now + (duration <= 0 ? minutes(1) : seconds(duration));
	for(int i = 0; i < n; i++) {
		if(next_t > flows[i].next_t){
			next_t = flows[i].next_t;
		}
	}
	
	try {
		if (busyWait) {
			while(NOW < next_t) {}
		} else if (NOW < next_t) {
			sleep_until(next_t);
		}
		
		while(next_t < end_t) {
			next_t = end_t;
			now = NOW;
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
					
					//NEXT PDU
					f.next_t = now + interval;
					if(next_t > f.next_t){
						next_t = f.next_t;
					}
				}
			}
			
			if (busyWait) {
				while(NOW < next_t) {}
			} else if (NOW < next_t) {
				sleep_until(next_t);
			}
		}
	} catch (...) {}
	
	try{
		release_flow(port_id);
	} catch(...){}
}
