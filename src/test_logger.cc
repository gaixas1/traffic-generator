#include "test_logger.h"

#ifndef RINA_PREFIX
#define RINA_PREFIX "LOGGER"
#endif // !RINA_PREFIX

#include <librina/librina.h>
#include <librina/logs.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <thread>

#include "test_common.h"

using namespace std;
using namespace std::chrono;
using namespace rina;

flow_log::flow_log() {
	count = 0; data = 0;
	maxLat = 0;
	latCount = 0;
}

void flow_log::process(long long t, dataSDU * sdu) {
	seq_id = sdu->seqId;

	count++;
	data += sdu->size;

	long long lat = t - sdu->send_time;
	if (lat > maxLat) maxLat = lat;
	latCount += lat;
}

qos_log::qos_log() {
	count = 0; data = 0; total = 0;
	maxLat = 0;
	latCount = 0;
}

void qos_log::process(flow_log * f) {
	count += f->count;
	total += f->seq_id+1;
	data += f->data;
	if (f->maxLat > maxLat) maxLat = f->maxLat;
	latCount += f->latCount;
}

test_logger::test_logger(std::string name, std::string api, bool _log)
	: simple_server(name, api), log(_log), count(0) {};

void test_logger::handle_flow(int port_id, int fd) {
	union {
		char buffer[BUFF_SIZE];
		dataSDU data;
	};
	try {
		if (!timed_read_dataSDU(fd, buffer, 1, 0) || data.type != DTYPE_INIT) {
			mt.lock();
			count--;
			mt.unlock();
			LOG_ERR("FIRST READ FAILED - ABORT FLOW");
			release_flow(port_id);
			return;
		}
		if (write(fd, buffer, data.size) != (int)data.size) {
			LOG_ERR("FAILED AT SENDING INIT MESSAGE - ABORT FLOW");
			release_flow(port_id);
			return;
		}
	} catch (...) {
		LOG_ERR("EXCEPTION CATCHED - Something failed");
		return;
	}

	if (log) {
		initSDU * init = (initSDU*) buffer + sizeof(dataSDU);
		bool start_logger;
		flow_log * flow = new flow_log();
		flow->QoSId = init->QoSId;
		flow->flowId = init->flowId;
		flow->seq_id = data.seqId;

		mt.lock();
		start_logger = (count == 0);
		flow_logs.push_back(flow);
		count++;
		mt.unlock();

		if (start_logger) {
			thread t(&test_logger::logger_t, this);
			t.detach();
		}

		while(timed_read_dataSDU(fd, buffer, 1, 0) && data.type == DTYPE_DATA) {
			flow->process(duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count(), &data);
		}

		mt.lock();
		count--;
		mt.unlock();

	} else {
		for (;;) {
			if (!timed_read_dataSDU(fd, buffer, 1, 0) || data.type != DTYPE_DATA) {
				break;
			}
		}
	}
	try{
		release_flow(port_id);
	} catch(...){}
}

void test_logger::logger_t() {
	chrono::seconds s5(5);
	std::this_thread::sleep_for(s5);
	do {
		mt.lock();
		if (count > 0) {
			std::cout << "Flows in process "<<  count <<  std::endl;
			mt.unlock();
			std::this_thread::sleep_for(s5);
		} else {
			break;
		}
	} while (true);

	std::cout << "Flows in process "<<  count <<  std::endl;
	std::cout << "Print log"<<  std::endl;
	
	//Process log
	long long count = 0, data = 0;

	std::map<int, qos_log> qos_logs;
	for (flow_log * f : flow_logs) {
		count += f->count;
		data += f->data;
		qos_logs[f->QoSId].process(f);
	}

	//Print log
	std::cout << std::endl;
	std::cout << count << " | " << (data / 125000.0) << " Mb" << std::endl;

	for (flow_log * f : flow_logs) {
		long long c = f->count;
		long long t = f->seq_id;
		long double l = t - c;

		std::cout << "\t" << f->flowId << " (" << (int)f->QoSId << ") | "
			<< c << " / " << t << " (" << (l*100.0 / c) << " %) | " << f->data << " B"
			<< " ||" << (f->maxLat / 1000.0) 
			<< " -- " << (f->latCount / (1000.0*c))
			<< std::endl;
	}


	for (auto qd : qos_logs) {
		int QoSId = qd.first;
		qos_log & q = qd.second;

		long long l = q.total - q.count;

		std::cout << "\t(" << QoSId  << ") | " 
			<< q.count << " | " << l << " | " << q.total << "  || "
			<< (q.maxLat / 1000.0) << "  | " << (q.latCount / (1000 * q.count));
		std::cout << "\t(" << QoSId << ")\t"
			<< (100.0*l / q.total) << " % || " 
			<< (q.latCount / (1000 * q.count)) << " -- " << (q.maxLat / 1000.0) << std::endl;

	}

	for (flow_log * f : flow_logs) {
		delete f;
	}

	count = 0;
	flow_logs.clear();
	mt.unlock();

}