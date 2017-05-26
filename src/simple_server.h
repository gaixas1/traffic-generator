#pragma once
#include "simple_ap.h"

class simple_server: public simple_ap {
public:
	simple_server(std::string name, std::string api)
		: simple_ap(name, api) {};
	void run();

protected:
	virtual void handle_flow(int port_id, int fd) = 0;
};
