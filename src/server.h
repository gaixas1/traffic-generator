#pragma once
#include "simple_server.h"

class server : public simple_server {
	public:
	server();
	~server();

protected:
	void handle_flow(int port_id, int fd);

private :
	bool print_interval;
	int stats_interval;

	bool flow(int fd, char * buffer, ofstream & log, bool echo, bool record);
};

