#pragma once

#include "simple_client.h"
#include "simple_server.h"

class ping_server : public simple_server {
public:
	ping_server(std::string name, std::string api) 
		: simple_server(name, api) {};

protected:
	void handle_flow(int port_id, int fd);
};

class ping_client : public simple_client {
public:
	ping_client(const std::string& apn, const std::string& api, const std::string& _dst_apn, const std::string& _dst_api, const vector<QoSpair>&_qos)
		: simple_client(apn, api, _dst_apn, _dst_api, _qos) {}

	void setMaxMsg(int max) { MAX_MSG = max; }

protected:
	int MAX_MSG;
	virtual void handle_flow(int port_id, int fd);
};
