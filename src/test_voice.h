#pragma once
#include "test_base.h"

class test_voice : public test_base {
public:
	test_voice(const std::string& apn, const std::string& api,
		const std::string& _dst_apn, const std::string& _dst_api, const vector<QoSpair>&_qos, int testduration,
		int _flowIdent, int _qosIdent, float HZ);

	void setON(unsigned int _sdu_size, int min_ms, int var_ms);
	void setOFF(unsigned int _sdu_size, int min_ms, int var_ms);

protected:
	bool flow();

	unsigned int on_sdu_size, off_sdu_size;
	int min_ms_on, var_ms_on;
	int min_ms_off, var_ms_off;

	nanoseconds interval;
	chrono::time_point<chrono::system_clock> change;
	bool isOn;
};

