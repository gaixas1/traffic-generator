#pragma once
#include "simple_ap.h"

class simple_client: public simple_ap {
public:
	simple_client(const std::string& apn, const std::string& api, const std::string& _dst_apn, const std::string& _dst_api, const vector<QoSpair>& _qos)
		: simple_ap(apn, api), dst_apn(_dst_apn), dst_api(_dst_api), qos(_qos) {}
	void run();

protected:
	std::string dst_apn, dst_api;
	const vector<QoSpair>& qos;
	virtual void handle_flow(int port_id, int fd) = 0;
};
