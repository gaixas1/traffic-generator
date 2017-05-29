#pragma once
#include "base_client.h"

class data_client : public base_client {
public:
	data_client(const std::string& apn, const std::string& api,
		const std::string& _dst_apn, const std::string& _dst_api, const vector<QoSpair>&_qos,
		long _timeDif, int _flowIdent, bool _doEcho, bool _doRecord, bool _busyWait) :
		base_client(apn, api, _dst_apn, _dst_api, _qos, _timeDif, _flowIdent, _doEcho, _busyWait){}

	void setBurstSize(int min, int max);

	void setData(int kB);

protected:
	bool flow(int fd, char * buffer);

	int MIN_BURST, DIF_BURST;
	int MIN_DATA;
};

