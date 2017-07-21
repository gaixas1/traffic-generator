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
	
	data.flowIdent = flowIdent;
	
	microseconds interval((int) (1000000.0/Hz));
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
	if (write(fd, buffer, data.len) != data.len) {	
		cout << "First PDU error!!!"<<endl;
		return;
	}
	
	* ((int*)bdata) = n;
	data.len = sizeof(SDU) + sizeof(int);
	data.t = duration_cast<milliseconds>(now.time_since_epoch()).count();
	
	if (write(fd, buffer, data.len) != data.len) {	
		cout << "Second PDU error!!!"<<endl;
		return;
	}
	
	
	now = system_clock::now();
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
	cout << MIN_PDU_ON << " " << DIF_PDU_ON  << " " << MIN_PDU_OFF  << " " << DIF_PDU_OFF <<endl;
	cout << minCountPDUOn << " " << difCountPDUOn  << " " << difCountPDUOff  << " " << difCountPDUOff <<endl;
	
	for(int i = 0; i < n; i++) {
		voice_flow f;
		f.id = i;
		f.next_sq = 0;
		
		int t = rand()%(MAX_ON + MAX_OFF);
		f.on = t < MAX_ON;
		f.next_t = now + microseconds(rand() % ((int) (1000000.0/Hz)));
		if(f.on) {
			f.rem = rand() % (minCountPDUOn + difCountPDUOn)+1;
		} else {
			f.rem = rand() % (minCountPDUOff + difCountPDUOff)+1;
		}
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
		while(next_t < end_t) {
			now = system_clock::now();
			next_t = end_t;
			for(int i = 0; i < n; i++) {
				voice_flow & f = flows[i];
				while(f.next_t <= now) {
					//SEND PDU
					if(f.on) {
						data.len = MIN_PDU_ON + (DIF_PDU_ON ? rand()%DIF_PDU_ON : 0);
					} else  {
						data.len = MIN_PDU_OFF + (DIF_PDU_OFF ? rand()%DIF_PDU_OFF : 0);
					}
					data.id = f.id;
					data.sq = f.next_sq++;
					data.t = duration_cast<milliseconds>(now.time_since_epoch()).count();
					f.rem--;
					if (write(fd, buffer, data.len) != data.len) {
						return;
					}
					
					//NEXT PDU
					if(f.rem <= 0) {
						if(f.on) {
							f.on = false;
							f.rem = minCountPDUOff + (minCountPDUOff ? rand()%minCountPDUOff : 0);
						} else {
							f.on = true;
							f.rem = minCountPDUOn + (minCountPDUOn ? rand()%minCountPDUOn : 0);
						}
					} 
					
					f.next_t += interval;
					
				}
				if(next_t > f.next_t){
					next_t = f.next_t;
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
