#include "simple_client.h"

void simple_client::run() {
	port_fd p_fd = request_flow(dst_apn, dst_api, qos);
	if (p_fd.port <= 0) {
		return;
	}

	handle_flow(p_fd.port, p_fd.fd);
}