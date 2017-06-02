#include "simple_client.h"


#ifndef RINA_PREFIX
#define RINA_PREFIX "SIMPLE_CLIENT"
#endif // !RINA_PREFIX

#include <librina/librina.h>
#include <librina/logs.h>

using namespace rina;

void simple_client::run() {
	port_fd p_fd = request_flow(dst_apn, dst_api, qos);
	if (p_fd.port <= 0) {
		return;
	}

	handle_flow(p_fd.port, p_fd.fd);
}