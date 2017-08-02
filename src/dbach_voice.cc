#include "dbach_voice.h"
#include <fstream>
#include <thread>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#ifndef RINA_PREFIX
#define RINA_PREFIX "DBACH_VOICE"
#endif // !RINA_PREFIX

#include <librina/librina.h>
#include <librina/logs.h>
#include "bcommon.h"

using namespace std;
using namespace std::chrono;
using namespace std::this_thread;
using namespace rina;

void dbach_voice::setPDUON(int min_B, int max_B) {
	if (min_B < (int)sizeof(SDU) ) {
		throw std::invalid_argument("received non-positive or \"< headers size\" value for PDU size");
	}
	if (min_B > max_B) {
		max_B = min_B;
	} 
	MIN_PDU_ON = min_B;
	MAX_PDU_ON = max_B;
}
void dbach_voice::setPDUOFF(int min_B, int max_B) {
	if (min_B < (int)sizeof(SDU) ) {
		cout << min_B << " vs "<< (int)sizeof(SDU)<<endl;
		throw std::invalid_argument("received non-positive or \"< headers size\" value for PDU size (silence)");
	}
	if (min_B > max_B) {
		max_B = min_B;
	} 
	MIN_PDU_OFF = min_B;
	MAX_PDU_OFF = max_B;
}
void dbach_voice::setON(int min_ms, int max_ms) {
	if (min_ms <= 0) {
		throw std::invalid_argument("received non-positive value for ON interval");
	}
	if (min_ms > max_ms) {
		max_ms = min_ms;
	} 
	MIN_ON = min_ms;
	MAX_ON = max_ms;
}
void dbach_voice::setOFF(int min_ms, int max_ms) {
	if (min_ms <= 0) {
		throw std::invalid_argument("received non-positive value for OFF interval");
	}
	if (min_ms > max_ms) {
		max_ms = min_ms;
	} 
	MIN_OFF = min_ms;
	MAX_OFF = max_ms;
}
void dbach_voice::setDuration(int ms) {
	duration = ms;
}

	
void dbach_voice::handle_flow(int port_id, int fd) {
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
	
	int DIF_PDU_ON, DIF_PDU_OFF, DIF_ON, DIF_OFF;
		DIF_PDU_ON = MAX_PDU_ON - MIN_PDU_ON;
		DIF_PDU_OFF = MAX_PDU_OFF - MIN_PDU_OFF;
		DIF_ON = MAX_ON - MIN_ON;
		DIF_OFF = MAX_OFF - MIN_OFF;
		
	int minCountPDUOn, difCountPDUOn, minCountPDUOff, difCountPDUOff;
		minCountPDUOn = MIN_ON * Hz / 1000.0;
		difCountPDUOn = DIF_ON * Hz / 1000.0;
		minCountPDUOff = MIN_OFF * Hz / 1000.0;
		difCountPDUOff = DIF_OFF * Hz / 1000.0;
	
	now = NOW;
	for(int i = 0; i < n; i++) {
		voice_flow f(i);
		f.on = (MAX_ON > rand()%(MAX_ON + MAX_OFF));
		f.next_t = now + microseconds(rand() % ((int) (1000000.0/Hz)));
		f.rem = 1 + rand() % (f.on ? (minCountPDUOn + difCountPDUOn) : (minCountPDUOff + difCountPDUOff));
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
					data.len = f.on ? (MIN_PDU_ON + (DIF_PDU_ON ? rand()%DIF_PDU_ON : 0)) : (MIN_PDU_OFF + (DIF_PDU_OFF ? rand()%DIF_PDU_OFF : 0));
					data.id = f.id;
					data.sq = f.next_sq++;
					data.t = duration_cast<milliseconds>(now.time_since_epoch()).count();
					f.rem--;
					cout << "S : " << data.len << " / "<< data.id << " / "<< data.sq<< " / "<< data.t <<endl;
					cout << (int)buffer[0] << "."<< (int)buffer[1] << "."<< (int)buffer[2] << "."<< (int)buffer[3] << "."<< endl;
					if (write(fd, buffer, data.len) != data.len) {
						cerr << "SEND ERROR!!!" <<endl;
						return;
					}
					
					//NEXT PDU
					if(f.rem <= 0) {
						f.rem = f.on ? (minCountPDUOff + (minCountPDUOff ? rand()%minCountPDUOff : 0)) : (minCountPDUOn + (minCountPDUOn ? rand()%minCountPDUOn : 0)); 
						f.on = !f.on;
					} 
					f.next_t += interval;
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
