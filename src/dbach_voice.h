#pragma once
#include "simple_client.h"

#include <vector>
#include <chrono>

using namespace std;
using namespace std::chrono;

struct voice_flow {
	int id;
	int rem;
	unsigned long long next_sq;
	bool on;
	system_clock::time_point next_t;
};


class dbach_voice : public simple_client {
public:
	dbach_voice(const std::string& apn, const std::string& api,
		const std::string& _dst_apn, const std::string& _dst_api, const vector<QoSpair>&_qos,
		int _flowIdent, double _Hz, bool _busyWait, int _n) :
		simple_client(apn, api, _dst_apn, _dst_api, _qos), flowIdent(_flowIdent), Hz(_Hz), busyWait(_busyWait), n(_n) {}

	void setPDUON(int min_B, int max_B  = 0);
	void setPDUOFF(int min_B, int max_B  = 0);
	void setON(int min_ms, int max_ms  = 0);
	void setOFF(int min_ms, int max_ms = 0);
	void setDuration(int s);

protected:
	int flowIdent;
	double Hz;
	bool busyWait;
	int n;
	long duration;
	int MIN_PDU_ON, MAX_PDU_ON, MIN_PDU_OFF, MAX_PDU_OFF, MIN_ON, MAX_ON, MIN_OFF, MAX_OFF;
	vector<voice_flow> flows;
	
	void handle_flow(int port_id, int fd);
};


