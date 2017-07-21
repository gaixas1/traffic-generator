#pragma once
#include "simple_server.h"

class logger : public simple_server {
	public:
		logger(std::string name, std::string api) : simple_server(name, api) {};
		void handle_flow(int port_id, int fd);
};

