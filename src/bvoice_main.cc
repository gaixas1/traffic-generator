#include <cstdlib>
#include <string>

#include <librina/librina.h>

#define RINA_PREFIX	"traffic-generator-bvoice"
#include <librina/logs.h>

#include "tclap/CmdLine.h"

#include "config.h"
#include "bach_voice.h"

using namespace std;
using namespace TCLAP;

int main(int argc, char * argv[]) {
	string	apName, apInstance, dstName, dstInstance, qos_file;
	vector<string> difs;
	vector<QoSpair> qos;
	
	bool busywait = true;
	double Hz = 1;
	int flows =1, duration = 0, minPDU = 50, maxPDU=150, minOn = 1000, maxOn = 1000, minOff = 1000, maxOff = 1000;


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

		ValueArg<double> hz_a(
			"H",
			"HZ",
			"Frame rate of flows, 50Hz",
			false,
			50.0,
			"double"
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
		
		ValueArg<int> minPDU_a(
			"p",
			"minPDU",
			"Minimum PDU size in B, default = 1000, min = 50",
			false,
			1000,
			"int"
		);

		ValueArg<int> maxPDU_a(
			"P",
			"maxPDU",
			"Maximum PDU size in B, default = 1000, min = 50",
			false,
			1000,
			"int"
		);

		ValueArg<int> minOn_a(
			"o",
			"min_on",
			"Min duration of ON interval in ms, default = 1000.",
			false,
			1000,
			"int"
		);

		ValueArg<int> maxOn_a(
			"O",
			"max_on",
			"Max duration of ON interval in ms, default = 1000.",
			false,
			1000,
			"int"
		);

		ValueArg<int> minOff_a(
			"f",
			"min_off",
			"Min duration of OFF interval in ms, default = 1000.",
			false,
			1000,
			"int"
		);

		ValueArg<int> maxOff_a(
			"F",
			"max_off",
			"Max duration of OFF interval in ms, default = 1000.",
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
		cmd.add(hz_a);
		cmd.add(minPDU_a);
		cmd.add(maxPDU_a);
		cmd.add(minOn_a);
		cmd.add(maxOn_a);
		cmd.add(minOff_a);
		cmd.add(maxOff_a);
		
		cmd.add(difs_a);
		cmd.parse(argc, argv);

		apName = apName_a.getValue();
		apInstance = apInstance_a.getValue();
		dstName = dstName_a.getValue();
		dstInstance = dstInstance_a.getValue();
		difs = difs_a.getValue();
		
		busywait = busywait_a.getValue();
		Hz = hz_a.getValue();
		if (Hz <= 0) { Hz = 1; }
		flows = f_a.getValue();
		if (flows < 1) { flows = 1; }
		duration = duration_a.getValue();
		minPDU = minPDU_a.getValue();
		if (minPDU < (int)sizeof(SDU)) { minPDU = (int)sizeof(SDU); }
		maxPDU = maxPDU_a.getValue();
		if (maxPDU < minPDU) { maxPDU = minPDU; }
		minOn = minOn_a.getValue();
		if (minOn < 1) { minOn = 1; }
		maxOn = maxOn_a.getValue();
		if (maxOn < minOn) { maxOn = minOn; }
		minOff = minOff_a.getValue();
		if (minOff < 1) { minOff = 1; }
		maxOff = maxOff_a.getValue();
		if (maxOn < minOff) { maxOn = minOff; }

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

		bach_voice c(apName, apInstance, dstName, dstInstance, qos, Hz, busywait, flows);
		c.register_ap(difs);
		c.setDuration(duration);
		c.setPDU(minPDU, maxPDU);
		c.setON(minOn, maxOn);
		c.setOFF(minOff, maxOff);
		c.run();
	}
	catch (rina::Exception& e) {
		LOG_ERR("%s", e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
