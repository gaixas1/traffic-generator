#pragma once
#include "simple_client.h"
#include "bcommon.h"
#include <vector>
#include <chrono>

using namespace std;
using namespace std::chrono;

struct _flow {
	short id;
	short next_sq;
	system_clock::time_point next_t;
	_flow(short _id) : id(_id), next_sq(0) {}
};


class batch_crate : public simple_client {
public:
	batch_crate(const std::string& apn, const std::string& api,
		const std::string& _dst_apn, const std::string& _dst_api, const vector<QoSpair>&_qos,
		int _bps, bool _busyWait, short _n) :
		simple_client(apn, api, _dst_apn, _dst_api, _qos), bps(_bps), busyWait(_busyWait), n(_n) {}

	void setPDU(int B);
	void setDuration(int s);

protected:
	int bps;
	bool busyWait;
	short n;
	long duration;
	int PDU_S;
	vector<_flow> flows;
	
	void handle_flow(int port_id, int fd);
};


