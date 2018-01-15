#include "test_base.h"

#include <fstream>
#include <thread>
#include <stdlib.h>
#include <time.h>
#include <stdexcept>


#ifndef RINA_PREFIX
#define RINA_PREFIX "DATA_CLIENT"
#endif // !RINA_PREFIX

#include <librina/librina.h>
#include <librina/logs.h>

using namespace std;
using namespace std::chrono;
using namespace std::this_thread;
using namespace rina;

void test_base::handle_flow(int port_id, int fd) {
	Fd = fd;
	srand(time(0));
	char buffer[BUFF_SIZE];
	data = (dataSDU*)buffer;
	initSDU * init = (initSDU*)buffer + MIN_PDU;

	data->size = MIN_PDU + sizeof(initSDU);
	data->type = DTYPE_INIT;
	data->seqId = 0;

	init->flowId = flowIdent;
	init->QoSId = qosIdent;

	try {
		if (write(fd, buffer, data->size) != (int) data->size) {
			LOG_ERR("FAILED AT SENDING INIT MESSAGE - ABORT FLOW");
			release_flow(port_id);
			return;
		}
		if (!timed_read_dataSDU(fd, buffer, 1, 0) || data->type != DTYPE_INIT) {
			LOG_ERR("FIRST READ FAILED - ABORT FLOW");
			release_flow(port_id);
			return;
		}
	} catch(...){
		LOG_ERR("EXCEPTION CATCHED - Something failed");
		return;
	}

	std::this_thread::sleep_for(chrono::seconds(1));

	endtime = chrono::system_clock::now() + chrono::seconds(duration);
	data->type = DTYPE_DATA;
	bool result = flow();

	std::this_thread::sleep_for(chrono::seconds(5));
	
	if(result) {
		data->type = DTYPE_FIN;
		data->size = sizeof(data);
		try {		
			if (write(fd, buffer, data->size) != (int)data->size) {
				LOG_ERR("FAILED AT SENDING INIT MESSAGE - ABORT FLOW");
				return;
			}	
		} catch(...){
			LOG_ERR("EXCEPTION CATCHED - Something failed");
			return;
		}
	}

	std::this_thread::sleep_for(chrono::seconds(5));

	release_flow(port_id);

	std::this_thread::sleep_for(chrono::seconds(5));
}


bool test_base::sendPDU(unsigned int size) {
	data->size = size;
	data->seqId++;
	data->send_time = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

	try {
		if (write(Fd, (char*)data, size) != (int)size) {
			LOG_ERR("FAILED AT SENDING DATA MESSAGE - ABORT FLOW");
			return false;
		}
	} catch (...) {
		LOG_ERR("EXCEPTION CATCHED - Write failed");
		return false;
	}
	return true;
}