#pragma once
#include <mutex>

using namespace std;

#include "simple_client.h"
class chat_client : public simple_client {
public:
	chat_client(const std::string& apn, const std::string& api, const std::string& _dst_apn, const std::string& _dst_api, const vector<QoSpair>&_qos)
		: simple_client(apn, api, _dst_apn, _dst_api, _qos), open(true) {}


	void handle_flow(int port_id, int fd);

protected:
	mutex mt;
	bool open;

	int doCommand(int fd, char * buffer, string msg);
	int sendPrivate(int fd, char * buffer, string msg);
	int sendMessage(int fd, char * buffer, string msg);
	void listener(int df);
};


