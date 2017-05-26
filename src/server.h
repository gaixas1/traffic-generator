#pragma once
#include "simple_server.h"

class server : public simple_server {
	public:
		server(std::string name, std::string api) : simple_server(name, api) {};

		void setInterval(bool stat, int ms);
		void setRecordRange(int ms);

protected:
	void handle_flow(int port_id, int fd);

private :
	bool print_interval;
	int stats_interval;
	long rangeLat;

	bool flow(int fd, char * buffer, ofstream & log, bool echo, bool record, ofstream & recordlog);
};

