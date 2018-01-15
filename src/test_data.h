#pragma once
#include "test_base.h"

class test_data : public test_base {
public:
	test_data(const std::string& apn, const std::string& api,
		const std::string& _dst_apn, const std::string& _dst_api, const vector<QoSpair>&_qos, int testduration,
		int _flowIdent, int _qosIdent, unsigned int _sdu_size, unsigned long long ratebps);

protected:
	bool flow();

	unsigned int sdu_size;
	nanoseconds interval;

};

