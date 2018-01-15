#include <cstdlib>
#include <string>

#include <librina/librina.h>

#define RINA_PREFIX	"traffic-generator-test-voice"
#include <librina/logs.h>

#include "tclap/CmdLine.h"

#include "config.h"
#include "test_voice.h"

using namespace std;
using namespace TCLAP;

int main(int argc, char * argv[]) {
	string	apName, apInstance, dstName, dstInstance, qos_file;
	vector<string> difs;
	vector<QoSpair> qos;
	int flowIdent;
	int qosIdent;
	float HZ;
	int duration;

	try {
		CmdLine cmd("test voice", ' ', PACKAGE_VERSION);

		ValueArg<string> apName_a(
			"n",
			"apName",
			"Application process name, default = traffic.generator.test.voice.",
			false,
			"traffic.generator.test.voice",
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
			"Destination Application process name, default = traffic.generator.test.no-log.",
			false,
			"traffic.generator.test.no-log",
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

		ValueArg<int> duration_a(
			"D",
			"duration",
			"Test duration in s, default = 10.",
			false,
			10,
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
		ValueArg<int> qosIdent_a(
			"q",
			"qosid",
			"QoS identifier, default = 0.",
			false,
			0,
			"int"
		);
		ValueArg<float> HZ_a(
			"H",
			"hz",
			"FPS, default = 50.0.",
			false,
			50.0f,
			"float"
		);


		cmd.add(apName_a);
		cmd.add(apInstance_a);
		cmd.add(dstName_a);
		cmd.add(dstInstance_a);
		cmd.add(qos_file_a);
		cmd.add(flowIdent_a);
		cmd.add(qosIdent_a);
		cmd.add(HZ_a);
		cmd.add(duration_a);
		cmd.add(difs_a);
		cmd.parse(argc, argv);

		apName = apName_a.getValue();
		apInstance = apInstance_a.getValue();
		dstName = dstName_a.getValue();
		dstInstance = dstInstance_a.getValue();
		difs = difs_a.getValue();
		flowIdent = flowIdent_a.getValue();
		qosIdent = qosIdent_a.getValue();
		HZ = HZ_a.getValue();
		duration = duration_a.getValue();

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

		test_voice c(apName, apInstance, dstName, dstInstance, qos, duration,
			flowIdent, qosIdent, HZ);
		c.register_ap(difs);
		c.run();
	}
	catch (rina::Exception& e) {
		LOG_ERR("%s", e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
