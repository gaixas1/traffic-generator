#pragma once
#include "simple_client.h"
#include <stdexcept>

class data_client : public simple_client {
public:
	data_client(const std::string& apn, const std::string& api, const std::string& _dst_apn, const std::string& _dst_api, const vector<QoSpair>&_qos)
		: simple_client(apn, api, _dst_apn, _dst_api, _qos),
		flowIdent(0), doEcho(false), doRecord(false){}

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
	void setBurstSize(int min, int max) {
		if (min == max) {
			MIN_BURST = min;
			DIF_BURST = 0;
		}
		else if (min < max) {
			MIN_BURST = min;
			DIF_BURST = max - min;
		}
		else {
			MIN_BURST = max;
			DIF_BURST = min - max;
		}
		if (MIN_BURST <= 0) {
			throw std::invalid_argument("received negative value for burst size");
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

protected:
	virtual void handle_flow(int port_id, int fd);
	bool flow(int fd, char * buffer);


	int flowIdent;
	bool doEcho;
	bool doRecord;
	int MIN_BURST, DIF_BURST, MIN_PDU, DIF_PDU, nsPDU;
	int MIN_DATA;
	long timeDIF;
};

