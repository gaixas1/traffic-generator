/*
 * Traffic generator
 *
 *   Addy Bombeke <addy.bombeke@ugent.be>
 *   Dimitri Staessens <dimitri.staessens@intec.ugent.be>
 *   Douwe De Bock <douwe.debock@ugent.be>
 *
 *   Edit by Sergio Leon <slgaixas@ac.upc.edu>
 *
 * This source code has been released under the GEANT outward license.
 * Refer to the accompanying LICENSE file for further information
 */

#include <errno.h>
#include <boost/thread.hpp>

#ifndef RINA_PREFIX
#define RINA_PREFIX "SIMPLE_SERVER"
#endif // !RINA_PREFIX

#include <librina/librina.h>
#include <librina/logs.h>

#include "simple_server.h"
#include "timeutils.h"


using namespace std;
using namespace rina;

void simple_server::run() {
	for(;;) {
		IPCEvent * event = ipcEventProducer->eventWait();
		if (event == nullptr) {
			return;
		}
		switch (event->eventType) {
			case REGISTER_APPLICATION_RESPONSE_EVENT:{
				ipcManager->commitPendingRegistration(event->sequenceNumber, dynamic_cast<RegisterApplicationResponseEvent*>(event)->DIFName);
				break;
			}
			case UNREGISTER_APPLICATION_RESPONSE_EVENT:{
				ipcManager->appUnregistrationResult(event->sequenceNumber, dynamic_cast<UnregisterApplicationResponseEvent*>(event)->result == 0);
				break;
			}
			case FLOW_ALLOCATION_REQUESTED_EVENT: {
				rina::FlowInformation flow = ipcManager->allocateFlowResponse( *dynamic_cast<FlowRequestEvent*>(event), 0, true);
				LOG_INFO("New flow allocated [port-id = %d]", flow.portId);
				boost::thread t(&server::handle_flow, this, flow.portId, flow.fd);
				t.detach();
				break;
			}
			case FLOW_DEALLOCATED_EVENT: {
				int port_id = dynamic_cast<FlowDeallocatedEvent*>(event)->portId;
				ipcManager->flowDeallocated(port_id);
				LOG_INFO("Flow torn down remotely [port-id = %d]", port_id);
				break;
			}
			default: {
				LOG_INFO("Server got new event of type %d", event->eventType);
				break;
			}
		}
	}
}
