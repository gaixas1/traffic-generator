#include <cstdlib>
#include <string>

#include <librina/librina.h>

#define RINA_PREFIX	"ping"
#include <librina/logs.h>

#include "tclap/CmdLine.h"

#include "config.h"
#include "ping.h"

using namespace std;
using namespace TCLAP;

int main(int argc, char * argv[]) {
	string	apName, apInstance, dstName, dstInstance, qos_file;
	vector<string> difs;
	vector<QoSpair> qos;
	int maxMsgs;

	try {
		CmdLine cmd("ping", ' ', PACKAGE_VERSION);

		ValueArg<string> apName_a(
			"n",
			"apName",
			"Application process name, default = traffic.generator.server.",
			false,
			"ping.server",
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
			"dsxtName",
			"Destination Application process name, default = \"\" (server mode).",
			false,
			"",
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

		ValueArg<int> maxMsgs_a(
			"M",
			"max_msgs",
			"Number of messages, default = 10 ns.",
			false,
			10,
			"int"
		);


		cmd.add(apName_a);
		cmd.add(apInstance_a);
		cmd.add(dstName_a);
		cmd.add(dstInstance_a);
		cmd.add(qos_file_a);
		cmd.add(maxMsgs_a);
		cmd.add(difs_a);
		cmd.parse(argc, argv);

		apName = apName_a.getValue();
		apInstance = apInstance_a.getValue();
		dstName = dstName_a.getValue();
		dstInstance = dstInstance_a.getValue();
		difs = difs_a.getValue();

		qos_file = qos_file_a.getValue();
		if (qos_file != "") {
			parseQoSRequirements(qos, qos_file);
		}
		
		maxMsgs = maxMsgs_a.getValue();
		if (maxMsgs <= 0) {
			maxMsgs = 1;
		}

	} catch (ArgException &e) {
		LOG_ERR("Error: %s for arg %d",
			e.error().c_str(),
			e.argId().c_str());
		exit(1);
	}

	try {
		rina::initialize("INFO", "");
		if (dstName == string("")) {
			cout << "Ping server | " << apName << ":" << apInstance <<endl;
			ping_server s(apName, apInstance);
			s.register_ap(difs);
			s.run();
		} else {
			cout << "Ping client | " << apName << ":" << apInstance << " => " << dstName << ":" << dstInstance  <<endl;
			ping_client c(apName, apInstance, dstName, dstInstance, qos);
			c.register_ap(difs);
			c.setMaxMsg(maxMsgs);
			c.run();
		}
	} catch (rina::Exception& e) {
		LOG_ERR("%s", e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
