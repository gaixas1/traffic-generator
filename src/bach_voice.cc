#include "bach_voice.h"
#include <fstream>
#include <thread>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#ifndef RINA_PREFIX
#define RINA_PREFIX "BACH_VOICE"
#endif // !RINA_PREFIX

#include <librina/librina.h>
#include <librina/logs.h>
#include "bcommon.h"

using namespace std;
using namespace std::chrono;
using namespace std::this_thread;
using namespace rina;

void bach_voice::setPDU(int min_B, int max_B) {
	if (min_B <= MIN_BUFF_SIZE ) {
		throw std::invalid_argument("received non-positive or \"< headers size\" value for PDU size");
	}
	if (min_B > max_B) {
		max_B = min_B;
	} 
	MIN_PDU = min_B;
	MAX_PDU = max_B;
}
void bach_voice::setON(int min_ms, int max_ms) {
	if (min_ms <= 0) {
		throw std::invalid_argument("received non-positive value for ON interval");
	}
	if (min_ms > max_ms) {
		max_ms = min_ms;
	} 
	MIN_ON = min_ms;
	MAX_ON = max_ms;
}
void bach_voice::setOFF(int min_ms, int max_ms) {
	if (min_ms <= 0) {
		throw std::invalid_argument("received non-positive value for OFF interval");
	}
	if (min_ms > max_ms) {
		max_ms = min_ms;
	} 
	MIN_OFF = min_ms;
	MAX_OFF = max_ms;
}
void bach_voice::setDuration(int ms) {
	duration = ms;
}

	
void bach_voice::handle_flow(int port_id, int fd) {
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
	
	int DIF_PDU, DIF_ON, DIF_OFF;
		DIF_PDU = MAX_PDU - MIN_PDU;
		DIF_ON = MAX_ON - MIN_ON;
		DIF_OFF = MAX_OFF - MIN_OFF;
	
	
	now = NOW;
	
	for(int i = 0; i < n; i++) {
		voice_flow f(i);
		f.rem = (int) ceil((MIN_ON + (DIF_ON ? rand()%DIF_ON : 0)) * Hz / 1000.0);
		f.next_t = now + milliseconds(rand() % ((MIN_OFF + MAX_OFF)/2));
		flows.push_back(f);
	}
	
	next_t = end_t = now + (duration <= 0 ? minutes(1) : seconds(duration));
	for(int i = 0; i < n; i++) {
		if(next_t > flows[i].next_t){
			next_t = flows[i].next_t;
		}
	}
	
	interval = microseconds((int) (1000000.0/Hz));
	try {
		while(next_t < end_t) {
			now = NOW;
			next_t = end_t;
			for(int i = 0; i < n; i++) {
				voice_flow & f = flows[i];
				while(f.next_t <= now) {
					//SEND PDU
					data.len = MIN_PDU + (DIF_PDU ? rand()%DIF_PDU : 0);
					data.id = f.id;
					data.sq = f.next_sq++;
					data.t = duration_cast<milliseconds>(now.time_since_epoch()).count();
					f.rem--;
					if (write(fd, buffer, data.len) != data.len) {
						return;
					}
					
					//NEXT PDU
					if(f.rem > 0) {
						f.next_t += interval;
					} else {
						f.rem = (int) ceil((MIN_ON + (DIF_ON ? rand()%DIF_ON : 0)) * Hz / 1000.0);
						f.next_t += milliseconds(MIN_OFF + (DIF_OFF ? rand()%DIF_OFF : 0));
					}
				}
				if(next_t > f.next_t){
					next_t = f.next_t;
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
