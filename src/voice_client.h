#pragma once
#include "simple_client.h"

class voice_client : public simple_client {
public:
	voice_client(const std::string& apn, const std::string& api, const std::string& _dst_apn, const std::string& _dst_api, const vector<QoSpair>&_qos)
		: simple_client(apn, api, _dst_apn, _dst_api, _qos), doEcho(false), doRecord(false) {}

	void setFlowIdent(int id) { flowIdent = id; }
	void setdoEcho(bool value) { doEcho = value; }
	void setdoRecord(bool value) { doRecord = value; }
	void setPDUSize(int min, int max) {
		if (min == max) {
			MIN_PDU = min;
			DIF_PDU = 0;
		}
		else if (min < max) {
			MIN_PDU = min;
			DIF_PDU = max - min;
		}
		else {
			MIN_PDU = max;
			DIF_PDU = min - max;
		}
		if (MIN_PDU <= 0) {
			throw std::invalid_argument("received negative value for PDU size");
		}
	}
	void setON(int min_ms, int max_ms) {
		if (min_ms == max_ms) {
			MIN_ON = min_ms;
			DIF_ON = 0;
		}
		else if (min_ms < max_ms) {
			MIN_ON = min_ms;
			DIF_ON = max_ms - min_ms;
		}
		else {
			MIN_ON = max_ms;
			DIF_ON = min_ms - max_ms;
		}
		if (MIN_ON <= 0) {
			throw std::invalid_argument("received negative value for ON interval");
		}
	}
	void setOFF(int min_ms, int max_ms) {
		if (min_ms == max_ms) {
			MIN_OFF = min_ms;
			DIF_OFF = 0;
		}
		else if (min_ms < max_ms) {
			MIN_OFF = min_ms;
			DIF_OFF = max_ms - min_ms;
		}
		else {
			MIN_OFF = max_ms;
			DIF_OFF = min_ms - max_ms;
		}
		if (MIN_OFF <= 0) {
			throw std::invalid_argument("received negative value for OFF interval");
		}
	}

	void setInterval(int ns) {
		nsPDU = ns;
	}
	void setData(int kB) {
		MIN_DATA = kB;
	}

	void setTimeDIF(long ns) {
		timeDIF = ns;
	}

	void setMinDuration(int s) {
		duration = s;
	}

	void setBusyWait(bool value) {
		busy_wait = value;
	}

protected:
	virtual void handle_flow(int port_id, int fd);
	bool flow(int fd, char * buffer);


	int flowIdent;
	bool doEcho;
	bool doRecord;
	int MIN_ON, DIF_ON, MIN_OFF, DIF_OFF, MIN_PDU, DIF_PDU, nsPDU;
	int MIN_DATA;
	long timeDIF;
	long duration;
	bool busy_wait;
};

