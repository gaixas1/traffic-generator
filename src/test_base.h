#pragma once
#include "simple_client.h"
#include <iostream>
#include <chrono>
#include "test_common.h"

#define MIN_PDU sizeof(dataSDU)

using namespace std::chrono;

class test_base : public simple_client {
public:
	test_base(const std::string& apn, const std::string& api,
			const std::string& _dst_apn, const std::string& _dst_api, const vector<QoSpair>&_qos,
			int _flowIdent, int _qosIdent, int testduration)
		: simple_client(apn, api, _dst_apn, _dst_api, _qos),
		flowIdent(_flowIdent), qosIdent(_qosIdent), duration(testduration){}

	void handle_flow(int port_id, int fd);

protected:
	virtual bool flow() = 0;
	dataSDU * data;
	chrono::time_point<chrono::system_clock> endtime;

	bool sendPDU(unsigned int size);

private:
	int flowIdent, qosIdent;
	int Fd;
	int duration;
};
