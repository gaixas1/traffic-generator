#pragma once
#include "simple_client.h"
#include "bcommon.h"

#include <vector>
#include <chrono>

using namespace std;
using namespace std::chrono;

struct voice_flow {
	short id;
	short next_sq;
	int rem;
	system_clock::time_point next_t;
	
	voice_flow(int _id) : id(_id), next_sq(0), rem(0){};
};


class bach_voice : public simple_client {
public:
	bach_voice(const std::string& apn, const std::string& api,
		const std::string& _dst_apn, const std::string& _dst_api, const vector<QoSpair>&_qos,
		double _Hz, bool _busyWait, short _n) :
		simple_client(apn, api, _dst_apn, _dst_api, _qos), Hz(_Hz), busyWait(_busyWait), n(_n) {}

	void setPDU(int min_B, int max_B  = 0);
	void setON(int min_ms, int max_ms  = 0);
	void setOFF(int min_ms, int max_ms = 0);
	void setDuration(int s);

protected:
	double Hz;
	bool busyWait;
	short n;
	long duration;
	int MIN_PDU, MAX_PDU, MIN_ON, MAX_ON, MIN_OFF, MAX_OFF;
	vector<voice_flow> flows;
	
	void handle_flow(int port_id, int fd);
};


