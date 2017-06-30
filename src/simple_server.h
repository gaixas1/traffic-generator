#pragma once
#include "simple_ap.h"

using namespace std;

class simple_server: public simple_ap {
public:
	simple_server(std::string name, std::string api)
		: simple_ap(name, api) {};
	void run();
	virtual void handle_flow(int port_id, int fd);
};
