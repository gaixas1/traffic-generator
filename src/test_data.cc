#include "test_data.h"
#include <fstream>
#include <thread>
#include <stdlib.h>
#include <time.h>

#ifndef RINA_PREFIX
#define RINA_PREFIX "test_data"
#endif // !RINA_PREFIX

#include <librina/librina.h>
#include <librina/logs.h>

using namespace std;
using namespace std::chrono;
using namespace std::this_thread;
using namespace rina;


test_data::test_data(const std::string& apn, const std::string& api,
	const std::string& _dst_apn, const std::string& _dst_api, const vector<QoSpair>&_qos, int testduration,
	int _flowIdent, int _qosIdent, unsigned int _sdu_size, unsigned long long ratebps, bool _cont) :
	test_base(apn, api, _dst_apn, _dst_api, _qos, _flowIdent, _qosIdent, testduration) {

	if (_sdu_size < MIN_PDU)
		sdu_size = MIN_PDU;
	else if (_sdu_size > BUFF_SIZE)
		sdu_size = BUFF_SIZE;
	else
		sdu_size = _sdu_size;

	cont = _cont;

	long long interval_ns;
	interval_ns = 1000000000L;		// ns/s
	interval_ns *= sdu_size * 8L;	// b/B
	interval_ns /= ratebps;			// b/s

	interval = nanoseconds(interval_ns);
}

bool test_data::flow() {
	chrono::time_point<chrono::system_clock> t = chrono::system_clock::now();
	while (t < endtime) {
		if (!sendPDU(sdu_size)) return false;
		if (cont) {
			t += interval;
		} else {
			t = chrono::system_clock::now() + interval;
		}
		sleep_until(t);
	};
	return true;
}
