#pragma once

#include "simple_client.h"
#include "simple_server.h"

class clocksincro_server : public simple_server {
public:
	clocksincro_server(std::string name, std::string api) 
		: simple_server(name, api) {};

protected:
	void handle_flow(int port_id, int fd);
};

class clocksincro_client : public simple_client {
public:
	clocksincro_client(const std::string& apn, const std::string& api, const std::string& _dst_apn, const std::string& _dst_api, const vector<QoSpair>&_qos)
		: simple_client(apn, api, _dst_apn, _dst_api, _qos) {}

	void setMaxMsg(int max) { MAX_MSG = max; }
	void setMaxSleep(int ms) { MAX_SLEEP = ms; }

protected:
	int MAX_MSG, MAX_SLEEP;
	virtual void handle_flow(int port_id, int fd);
};
