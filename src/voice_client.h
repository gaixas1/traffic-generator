#pragma once
#include "simple_client.h"

class voice_client : public simple_client {
public:
	voice_client(const std::string& apn, const std::string& api, const std::string& _dst_apn, const std::string& _dst_api, const vector<QoSpair>&_qos)
		: simple_client(apn, api, _dst_apn, _dst_api, _qos) {}

protected:
	virtual void handle_flow(int port_id, int fd);
};
