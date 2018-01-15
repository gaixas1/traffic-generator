#include "test_voice.h"
#include <fstream>
#include <thread>
#include <stdlib.h>
#include <time.h>

#ifndef RINA_PREFIX
#define RINA_PREFIX "test_voice"
#endif // !RINA_PREFIX

#include <librina/librina.h>
#include <librina/logs.h>

using namespace std;
using namespace std::chrono;
using namespace std::this_thread;
using namespace rina;

test_voice::test_voice(const std::string& apn, const std::string& api,
	const std::string& _dst_apn, const std::string& _dst_api, const vector<QoSpair>&_qos, int testduration,
	int _flowIdent, int _qosIdent, float HZ) :
	test_base(apn, api, _dst_apn, _dst_api, _qos, _flowIdent, _qosIdent, testduration) {

	setON(320, 3000, 1000);
	setOFF(20, 6000, 2000);

	long long interval_ns;
	interval_ns = 1000000000L;		// ns/s
	interval_ns /= HZ;				// 1/s

	interval = nanoseconds(interval_ns);
}

void test_voice::setON(unsigned int _sdu_size, int min_ms, int var_ms) {
	if (_sdu_size < MIN_PDU)
		on_sdu_size = MIN_PDU;
	else if (_sdu_size > BUFF_SIZE)
		on_sdu_size = BUFF_SIZE;
	else
		on_sdu_size = _sdu_size;
	min_ms_on = min_ms;
	var_ms_on = var_ms;
}

void test_voice::setOFF(unsigned int _sdu_size, int min_ms, int var_ms) {
	if (_sdu_size < MIN_PDU)
		off_sdu_size = MIN_PDU;
	else if (_sdu_size > BUFF_SIZE)
		off_sdu_size = BUFF_SIZE;
	else
		off_sdu_size = _sdu_size;
	min_ms_off = min_ms;
	var_ms_off = var_ms;
}

bool test_voice::flow() {
	unsigned int sdu_size;
	chrono::time_point<chrono::system_clock> t = chrono::system_clock::now();

	isOn = (rand() % (min_ms_on + min_ms_off) <= min_ms_on);
	if (isOn) {
		sdu_size = on_sdu_size;
		change = t + milliseconds(rand() % min_ms_on);
	} else {
		sdu_size = off_sdu_size;
		change = t + milliseconds(rand() % min_ms_off);
	}

	while (t < endtime) {
		if (t > change) {
			if (isOn) {
				isOn = false;
				sdu_size = off_sdu_size;
				change = t + milliseconds(min_ms_off + rand() % var_ms_off);
			} else {
				isOn = true;
				sdu_size = on_sdu_size;
				change = t + milliseconds(min_ms_on + rand() % var_ms_on);
			}
		}

		if (!sendPDU(sdu_size)) return false;
		t += interval;
		sleep_until(t);
	};

	return true;
}