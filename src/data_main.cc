#include <cstdlib>
#include <string>

#include <librina/librina.h>

#define RINA_PREFIX	"traffic-generator-server"
#include <librina/logs.h>

#include "tclap/CmdLine.h"

#include "config.h"
#include "data_client.h"

using namespace std;
using namespace TCLAP;

int main(int argc, char * argv[]) {
	string	apName, apInstance, dstName, dstInstance, qos_file;
	vector<string> difs;
	vector<QoSpair> qos;
	int		interval_duration;
	int timeDif, flowIdent;
	bool echo, record, busywait;
	int minPDU, maxPDU, interPDU;
	int data, minburst, maxburst;


	try {
		CmdLine cmd("traffic-generator-server", ' ', PACKAGE_VERSION);

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

		ValueArg<int> interval_duration_a(
			"R",
			"record_interval",
			"Record statistics at intervals of X ms, default = 0 (do not record).",
			false,
			0,
			"int"
		);

		ValueArg<int> timeDif_a(
			"t",
			"timedif",
			"Time difference in ns between client and server clocks, default = 0 ns.",
			false,
			0,
			"int"
		);

		ValueArg<int> flowIdent_a(
			"I",
			"flowid",
			"Unique flow identifier, default = 0.",
			false,
			0,
			"int"
		);

		SwitchArg echo_a(
			"e",
			"echo",
			"Request echo from server, default = false",
			false
		);
		SwitchArg record_a(
			"r",
			"record",
			"Record response from server, default = false",
			false
		);
		SwitchArg busywait_a(
			"w",
			"busywait",
			"Busy wait between bursts or sleep, default = false (sleep)",
			false
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

		ValueArg<int> interPDU_a(
			"v",
			"interval",
			"Time between PDUs in ns, default = 0",
			false,
			0,
			"int"
		);


		ValueArg<int> data_a(
			"D",
			"data",
			"kB of data to send, default = 1000",
			false,
			1000,
			"int"
		);

		ValueArg<int> minburst_a(
			"b",
			"minburst",
			"Min burst size, default = 1, min = 1.",
			false,
			1,
			"int"
		);

		ValueArg<int> maxburst_a(
			"B",
			"maxburst",
			"Max burst size, default = 1, min = 1.",
			false,
			1,
			"int"
		);


		cmd.add(apName_a);
		cmd.add(apInstance_a);
		cmd.add(dstName_a);
		cmd.add(dstInstance_a);
		cmd.add(qos_file_a);
		cmd.add(interval_duration_a);
		cmd.add(timeDif_a);
		cmd.add(flowIdent_a);
		cmd.add(echo_a);
		cmd.add(record_a);
		cmd.add(busywait_a);
		cmd.add(minPDU_a);
		cmd.add(maxPDU_a);
		cmd.add(interPDU_a);
		cmd.add(data_a);
		cmd.add(minburst_a);
		cmd.add(maxburst_a);
		cmd.add(difs_a);
		cmd.parse(argc, argv);

		apName = apName_a.getValue();
		apInstance = apInstance_a.getValue();
		dstName = dstName_a.getValue();
		dstInstance = dstInstance_a.getValue();
		difs = difs_a.getValue();
		interval_duration = interval_duration_a.getValue();
		timeDif = timeDif_a.getValue();
		flowIdent = flowIdent_a.getValue();
		echo = echo_a.getValue();
		record = record_a.getValue();
		busywait = busywait_a.getValue();
		minPDU = minPDU_a.getValue();
		if (minPDU < 50) { minPDU = 50; }
		maxPDU = maxPDU_a.getValue();
		if (maxPDU < minPDU) { maxPDU = minPDU; }
		interPDU = interPDU_a.getValue();
		if (interPDU < 0) { interPDU = 0; }
		data = data_a.getValue();
		if (data < 1) { data = 1; }
		minburst = minburst_a.getValue();
		if (minburst < 1) { minburst = 1; }
		maxburst = maxburst_a.getValue();
		if (maxburst < minburst) { maxburst = minburst; }

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

		data_client c(apName, apInstance, dstName, dstInstance, qos, timeDif, flowIdent, echo, record, busywait);
		c.register_ap(difs);
		c.setPDUSize(minPDU, maxPDU);
		c.setInterval(interPDU);
		c.setRecordInterval(interval_duration > 0, interval_duration);
		c.setData(data);
		c.setBurstSize(minburst, maxburst);
		c.run();
	}
	catch (rina::Exception& e) {
		LOG_ERR("%s", e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
