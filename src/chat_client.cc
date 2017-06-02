#include "chat_client.h"
#include <iostream>


#ifndef RINA_PREFIX
#define RINA_PREFIX "CHAT"
#endif // !RINA_PREFIX

#include <librina/librina.h>
#include <librina/logs.h>

#include "chat_common.h"

using namespace std;
using namespace rina;

void chat_client::handle_flow(int port_id, int fd) {

}
