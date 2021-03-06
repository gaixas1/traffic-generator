/*
 * Simple Application Process
 *
 *   Addy Bombeke	   <addy.bombeke@ugent.be>
 *   Douwe De Bock	   <douwe.debock@ugent.be>
 *   Francesco Salvestrini <f.salvestrini@nextworks.it>
 *
 *   Edit by Sergio Leon <slgaixas@ac.upc.edu>
 *
 * This source code has been released under the GEANT outward license.
 * Refer to the accompanying LICENSE file for further information
 */



#ifndef RINA_PREFIX
#define RINA_PREFIX "SIMPLE_APP"
#endif // !RINA_PREFIX

#include <librina/librina.h>
#include <librina/logs.h>

#include "simple_ap.h"
#include <fstream>
#include <iostream>


using namespace std;
using namespace rina;

simple_ap::~simple_ap(){ }

bool simple_ap::register_ap() {
	return register_ap("");
}

bool simple_ap::register_ap(const string & dif_name) {
	ApplicationRegistrationInformation ari;
	RegisterApplicationResponseEvent * resp;
	unsigned int seqnum;
	IPCEvent * event;

	ari.ipcProcessId = 0;  // This is an AP, not an IPC process
	ari.appName = ApplicationProcessNamingInformation(
		this->name,
		this->instance);
	if (dif_name == "") {
		ari.applicationRegistrationType = APPLICATION_REGISTRATION_ANY_DIF;
	} else {
		ari.applicationRegistrationType = APPLICATION_REGISTRATION_SINGLE_DIF;
		ari.difName = ApplicationProcessNamingInformation( dif_name, "");
	}

	// Request the registration
	seqnum = ipcManager->requestApplicationRegistration(ari);

	// Wait for the response to come
	for (;;) {
		event = ipcEventProducer->eventWait();
		if (event && event->eventType == REGISTER_APPLICATION_RESPONSE_EVENT && event->sequenceNumber == seqnum) {
			resp = dynamic_cast<RegisterApplicationResponseEvent*>(event);
			// Update librina state
			if (resp->result == 0) {
				ipcManager->commitPendingRegistration(seqnum, resp->DIFName);
				this->reg_difs.push_back(dif_name);
				return true;
			} else {
				ipcManager->withdrawPendingRegistration(seqnum);
				LOG_ERR("Failed to register application");
				return false;
			}
		}
	}
}

bool simple_ap::register_ap(const vector<string>& dif_names) {
	if(dif_names.empty()){
		return register_ap("");
	}
	
	for (unsigned int i = 0; i < dif_names.size(); i++) {
		if (!register_ap(dif_names[i])) {
			return false;
		}
	}
	return true;
}

bool simple_ap::unregister_ap() {
	return unregister_ap(this->reg_difs);
}

/* FIXME: implement this stub */
bool simple_ap::unregister_ap(const string& dif_name) {
	//throw exception("Not Implemented");
	return true;
}

bool simple_ap::unregister_ap(const vector<string>& dif_names) {
	for (unsigned int i=0; i< dif_names.size(); i++) {
		if (!unregister_ap(dif_names[i])) {
			return false;
		}
	}
	return true;
}

FlowSpecification getQoS(const vector<QoSpair> & rq) {
	FlowSpecification qos_spec;

	for (const QoSpair & fv : rq) {
		if (fv.param == "reliable") {
			qos_spec.maxAllowableGap = fv.value == "true" ? 0 : -1;
		} else if (fv.param == "partialDelivery") {
			qos_spec.partialDelivery = fv.value == "true" ? false : true;
		} else  if (fv.param == "orderedDelivery") {
			qos_spec.orderedDelivery = fv.value == "true" ? false : true;
		} else if (fv.param == "maxAllowableGap") {
			qos_spec.maxAllowableGap = stoi(fv.value);
		} else if (fv.param == "delay") {
			qos_spec.delay = stoi(fv.value);
		} else if (fv.param == "jitter") {
			qos_spec.jitter = stoi(fv.value);
		} else if (fv.param == "maxSDUsize") {
			qos_spec.maxSDUsize = stoi(fv.value);
		} else if (fv.param == "averageBandwidth") {
			qos_spec.averageBandwidth = stoi(fv.value);
			if(qos_spec.averageBandwidth<0) {
				qos_spec.averageBandwidth = 0;
			}
		} else if (fv.param == "averageSDUBandwidth") {
			qos_spec.averageSDUBandwidth = stoi(fv.value);
			if(qos_spec.averageSDUBandwidth<0) {
				qos_spec.averageSDUBandwidth = 0;
			}
		} else if (fv.param == "peakBandwidthDuration") {
			qos_spec.peakBandwidthDuration = stoi(fv.value);
			if(qos_spec.peakBandwidthDuration<0) {
				qos_spec.peakBandwidthDuration = 0;
			}
		} else if (fv.param == "peakSDUBandwidthDuration") {
			qos_spec.peakSDUBandwidthDuration = stoi(fv.value);
			if(qos_spec.peakSDUBandwidthDuration<0) {
				qos_spec.peakSDUBandwidthDuration = 0;
			}
		} else if (fv.param == "undetectedBitErrorRate") {
			qos_spec.undetectedBitErrorRate = stod(fv.value);
		} 
	}

	cout << qos_spec.toString()<<endl;
	return qos_spec;
}

port_fd simple_ap::request_flow(const std::string& apn, const std::string& api, const vector<QoSpair> & rq) {
	
	unsigned int seqnum = ipcManager->requestFlowAllocation(ApplicationProcessNamingInformation(this->name, this->instance), ApplicationProcessNamingInformation(apn, api), getQoS(rq));
	for (;;) {
		IPCEvent * event = ipcEventProducer->eventWait();
		if (event && event->eventType == ALLOCATE_FLOW_REQUEST_RESULT_EVENT && event->sequenceNumber == seqnum) {
			AllocateFlowRequestResultEvent * afrrevent = dynamic_cast<AllocateFlowRequestResultEvent*>(event);
			rina::FlowInformation flow =
				ipcManager->commitPendingFlow( afrrevent->sequenceNumber, afrrevent->portId, afrrevent->difName);
			if (flow.portId < 0) {
				LOG_ERR("Failed to allocate a flow");
				return port_fd(0, 0);
			}
			LOG_DBG("Port id = %d", flow.portId);
			return port_fd(flow.portId, flow.fd);
		}
		LOG_DBG("Got new event %d", event->eventType);
	}
}

port_fd simple_ap::request_flow(const std::string& apn, const std::string& api, const vector<QoSpair> & rq, const std::string& dif_name) {
	if (dif_name == "") {
		return request_flow(apn, api, rq);
	}

	
	unsigned int seqnum = ipcManager->requestFlowAllocationInDIF(ApplicationProcessNamingInformation( this->name, this->instance), ApplicationProcessNamingInformation( apn, api), ApplicationProcessNamingInformation(dif_name, ""),getQoS(rq));
	for (;;) {
		IPCEvent * event = ipcEventProducer->eventWait();
		if (event && event->eventType == ALLOCATE_FLOW_REQUEST_RESULT_EVENT && event->sequenceNumber == seqnum) {
			AllocateFlowRequestResultEvent * afrrevent = dynamic_cast<AllocateFlowRequestResultEvent*>(event);
			rina::FlowInformation flow = ipcManager->commitPendingFlow( afrrevent->sequenceNumber, afrrevent->portId, afrrevent->difName);
			if (flow.portId < 0) {
				LOG_ERR("Failed to allocate a flow");
				return port_fd(0, 0);
			}
			LOG_DBG("Port id = %d", flow.portId);
			return port_fd(flow.portId, flow.fd);
		}
		LOG_DBG("Got new event %d", event->eventType);
	}
}

/* FIXME, correct return value, update my_flows */
int simple_ap::release_flow(const int port_id) {
	ipcManager->deallocate_flow(port_id);
	return 0;
	/*
	unsigned int seqNum = ipcManager->requestFlowDeallocation(port_id);
	for (;;) {
		IPCEvent * event = ipcEventProducer->eventWait();
		if (event && event->eventType == DEALLOCATE_FLOW_RESPONSE_EVENT && event->sequenceNumber == seqNum) {
			DeallocateFlowResponseEvent * resp = dynamic_cast<DeallocateFlowResponseEvent*>(event);
			ipcManager->flowDeallocationResult(port_id, resp->result == 0);
			return 0;
		}
		LOG_DBG("Got new event %d", event->eventType);
	}
	*/
}

/* TODO: implement this stub */
int simple_ap::release_all_flows() {
	return 0;
}

int simple_ap::readBuffer(int fd, char * buffer, int bytes){
	do {
		int ret = read(fd, buffer, bytes);
		if (ret <= 0) {
			if(ret == 0) {
				LOG_ERR("read() failed: return 0");
			} else {
				LOG_ERR("read() failed: %s", strerror(errno));
			}
			return bytes;
		}
		bytes -= ret;
		buffer += ret;
	} while (bytes > 0);
	return bytes;
}


bool parseQoSRequirements(vector<QoSpair> & qos, string filename) {
	ifstream f(filename);
	if (!f.is_open()) {
		return false;
	}
	string a, b;
	while (f >> a >> b) {
		if (a != "" && b != "") {
			qos.push_back(QoSpair(a, b));
		}
	}
	f.close();
	return true;
}