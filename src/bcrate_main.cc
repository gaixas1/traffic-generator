#include <cstdlib>
#include <string>

#include <librina/librina.h>

#define RINA_PREFIX	"traffic-generator-bvoice"
#include <librina/logs.h>

#include "tclap/CmdLine.h"

#include "config.h"
#include "batch_crate.h"

using namespace std;
using namespace TCLAP;

int main(int argc, char * argv[]) {
	string	apName, apInstance, dstName, dstInstance, qos_file;
	vector<string> difs;
	vector<QoSpair> qos;
	
	bool busywait = true;
	int bps = 1;
	int flows =1, duration = 0, PDU_S = 1000;


	try {
		CmdLine cmd("traffic-generator-bvoice", ' ', PACKAGE_VERSION);

		ValueArg<string> apName_a(
			"n",
			"apName",
			"Application process name, default = traffic.generator.client.data.",
			false,
			"traffic.generator.client.data",
			"string"
		);
		ValueArg<string> apInstance_a(
			"i",
			"apInstance",
			"Application process instance, default = 1.",
			false,
			"1",
			"string"
		);
		ValueArg<string> dstName_a(
			"m",
			"dstName",
			"Destination Application process name, default = traffic.generator.server.",
			false,
			"traffic.generator.server",
			"string"
		);
		ValueArg<string> dstInstance_a(
			"j",
			"dstInstance",
			"Destination Application process instance, default = 1.",
			false,
			"1",
			"string"
		);
		ValueArg<string> qos_file_a(
			"Q",
			"qosfile",
			"File containing the QoS requirements (required)",
			false,
			"",
			"string"
		);
		UnlabeledMultiArg<string> difs_a(
			"difs",
			"DIFs to use, empty for any DIF.",
			false,
			"string"
		);

		
		SwitchArg busywait_a(
			"w",
			"busywait",
			"Busy wait between bursts or sleep, default = false (sleep)",
			false
		);

		ValueArg<int> bps_a(
			"b",
			"bps",
			"Rate in bps, default 1000",
			false,
			1000.0,
			"int"
		);
		
		ValueArg<int> f_a(
			"N",
			"numFlows",
			"Number of flows, default 1",
			false,
			1,
			"int"
		);
		
		ValueArg<int> duration_a(
			"D",
			"duration",
			"Duration of the test in s, default = 60s",
			false,
			60,
			"int"
		);
		
		ValueArg<int> PDU_a(
			"p",
			"pdus",
			"PDU size in B, default = 1000",
			false,
			1000,
			"int"
		);

		cmd.add(apName_a);
		cmd.add(apInstance_a);
		cmd.add(dstName_a);
		cmd.add(dstInstance_a);
		cmd.add(qos_file_a);
		
		cmd.add(busywait_a);
		cmd.add(duration_a);
		cmd.add(f_a);
		cmd.add(bps_a);
		cmd.add(PDU_a);
		
		cmd.add(difs_a);
		cmd.parse(argc, argv);

		apName = apName_a.getValue();
		apInstance = apInstance_a.getValue();
		dstName = dstName_a.getValue();
		dstInstance = dstInstance_a.getValue();
		difs = difs_a.getValue();
		
		busywait = busywait_a.getValue();
		bps = bps_a.getValue();
		if (bps <= 0) { bps = 1; }
		flows = f_a.getValue();
		if (flows < 1) { flows = 1; }
		duration = duration_a.getValue();
		PDU_S = PDU_a.getValue();
		if (PDU_S < (int)sizeof(SDU)) { PDU_S = (int)sizeof(SDU); }

		qos_file = qos_file_a.getValue();
		if (qos_file != "") {
			parseQoSRequirements(qos, qos_file);
		}

	}
	catch (ArgException &e) {
		LOG_ERR("Error: %s for arg %d",
			e.error().c_str(),
			e.argId().c_str());
		exit(1);
	}
	try {
		rina::initialize("INFO", "");

		batch_crate c(apName, apInstance, dstName, dstInstance, qos, bps, busywait, flows);
		c.register_ap(difs);
		c.setDuration(duration);
		c.setPDU(PDU_S);
		c.run();
	}
	catch (rina::Exception& e) {
		LOG_ERR("%s", e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
