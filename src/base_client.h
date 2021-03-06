#pragma once
#include "simple_client.h"
#include <iostream>
#include "common.h"

class base_client : public simple_client {
public:
	base_client(const std::string& apn, const std::string& api, 
			const std::string& _dst_apn, const std::string& _dst_api, const vector<QoSpair>&_qos,
			long _timeDif, int _flowIdent, bool _doEcho, bool _doRecord, bool _busyWait)
		: simple_client(apn, api, _dst_apn, _dst_api, _qos),
		timeDIF(_timeDif), flowIdent(_flowIdent), doEcho(_doEcho), doRecord(_doRecord), busyWait(_busyWait), MIN_PDU(sizeof(dataSDU)), DIF_PDU(0), nsPDU(1), print_interval(false), stats_interval(1000) {}

	void setPDUSize(int min, int max);
	void setInterval(int ns);
	void setRecordInterval(bool stat, int ms);

	void handle_flow(int port_id, int fd);

protected:
	virtual bool flow(int fd, char * buffer) = 0;

	long timeDIF;
	int flowIdent;
	bool doEcho, doRecord, busyWait;
	int MIN_PDU, DIF_PDU, nsPDU;
	bool print_interval;
	int stats_interval;
};


bool echo_listener(int fd, std::string filename, bool interval_stats, int interval_ms);
bool fin_listener(int fd);