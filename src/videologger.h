#pragma once
#include "simple_server.h"

class videologger : public simple_server {
	public:
		videologger(std::string name, std::string api, int _minBuffer, int _maxBuffer, double _playHz) : simple_server(name, api), minBuffer(_minBuffer), maxBuffer(_maxBuffer), playHz(_playHz) {};
		void handle_flow(int port_id, int fd);
		
	protected:
		int minBuffer, maxBuffer;
		double playHz;
};

