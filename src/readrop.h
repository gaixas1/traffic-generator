#pragma once
#include "simple_server.h"

class readrop : public simple_server {
	public:
		readrop(std::string name, std::string api) : simple_server(name, api) {};
		void handle_flow(int port_id, int fd);
};

