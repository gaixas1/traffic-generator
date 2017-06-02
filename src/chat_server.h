#pragma once
#include "simple_server.h"

class chat_server : public simple_server {
public:
	chat_server(std::string name, std::string api)
		: simple_server(name, api) {};

	void handle_flow(int port_id, int fd);
};

