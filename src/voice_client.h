#pragma once
#include "base_client.h"

class voice_client : public base_client {
public:
	voice_client(const std::string& apn, const std::string& api,
		const std::string& _dst_apn, const std::string& _dst_api, const vector<QoSpair>&_qos,
		long _timeDif, int _flowIdent, bool _doEcho, bool _doRecord, bool _busyWait) :
		base_client(apn, api, _dst_apn, _dst_api, _qos, _timeDif, _flowIdent, _doEcho, _doRecord, _busyWait) {}

		
		
	void setON(int min_ms, int max_ms);
	void setOFF(int min_ms, int max_ms);
	void setDuration(int ms);

protected:
	bool flow(int fd, char * buffer);

	int MIN_ON, DIF_ON, MIN_OFF, DIF_OFF;
	long duration;
};

