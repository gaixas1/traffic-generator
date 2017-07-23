#pragma once
#include "simple_server.h"

class datalogger : public simple_server {
	public:
		datalogger(std::string name, std::string api, int _countToLog) : simple_server(name, api), countToLog(_countToLog) {};
		void handle_flow(int port_id, int fd);
		
	protected:
		int countToLog;
};

