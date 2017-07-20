#pragma once
#include "simple_client.h"

#include <vector>
#include <chrono>

using namespace std;
using namespace std::chrono;

struct _flow {
	int id;
	unsigned long long next_sq;
	system_clock::time_point next_t;
};


class batch_crate : public simple_client {
public:
	batch_crate(const std::string& apn, const std::string& api,
		const std::string& _dst_apn, const std::string& _dst_api, const vector<QoSpair>&_qos,
		int _flowIdent, int _bps, bool _busyWait, int _n) :
		simple_client(apn, api, _dst_apn, _dst_api, _qos), flowIdent(_flowIdent), bps(_bps), busyWait(_busyWait), n(_n) {}

	void setPDU(int B);
	void setDuration(int s);

protected:
	int flowIdent;
	int bps;
	bool busyWait;
	int n;
	long duration;
	int PDU_S;
	vector<_flow> flows;
	
	void handle_flow(int port_id, int fd);
};


