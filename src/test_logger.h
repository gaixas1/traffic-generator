#pragma once
#include "simple_server.h"
#include "test_common.h"

#include <mutex>
#include <map>
#include <vector>

struct flow_log {
	flow_log();
	void process(long long t, dataSDU * sdu);

	int QoSId;
	int flowId;

	int seq_id;
	long long count, data;

	long long maxLat;
	long double latCount;
};
struct qos_log {
	qos_log();
	void process(flow_log * f);

	long long count, total, data;

	long long maxLat;
	long double latCount;
};

class test_logger : public simple_server {
	public:
		test_logger(std::string name, std::string api, bool _log);

		void handle_flow(int port_id, int fd);

	protected:
		bool log;
		int count;

		std::vector<flow_log*> flow_logs;

		mutex mt;

		void logger_t();
};

