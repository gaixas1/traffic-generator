#include <cstdlib>
#include <string>

#include <librina/librina.h>

#define RINA_PREFIX	"chat"
#include <librina/logs.h>

#include "tclap/CmdLine.h"

#include "config.h"
#include "chat_client.h"
#include "chat_server.h"

using namespace std;
using namespace TCLAP;

int main(int argc, char * argv[]) {
	string	apName, apInstance, dstName, dstInstance;
	vector<string> difs;
	vector<QoSpair> qos;
	int maxMsgs, maxSleep;

	try {
		CmdLine cmd("rina-chat", ' ', PACKAGE_VERSION);

		ValueArg<string> apName_a(
			"n",
			"apName",
			"Application process name, default = chat.server.",
			false,
			"traffic.generator.server",
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
		UnlabeledMultiArg<string> difs_a(
			"difs",
			"DIFs to use, empty for any DIF.",
			false,
			"string"
		);

		cmd.add(apName_a);
		cmd.add(apInstance_a);
		cmd.add(dstName_a);
		cmd.add(dstInstance_a);
		cmd.add(difs_a);
		cmd.parse(argc, argv);

		apName = apName_a.getValue();
		apInstance = apInstance_a.getValue();
		dstName = dstName_a.getValue();
		dstInstance = dstInstance_a.getValue();
		difs = difs_a.getValue();
	}
	catch (ArgException &e) {
		LOG_ERR("Error: %s for arg %d",
			e.error().c_str(),
			e.argId().c_str());
		exit(1);
	}

	try {
		if (dstName != "") {
			chat_server s(apName, apInstance);
			s.register_ap(difs);
			s.run();
		}
		else {
			chat_client c(apName, apInstance, dstName, dstInstance, qos);
			c.register_ap(difs);
			c.run();
		}
	}
	catch (rina::Exception& e) {
		LOG_ERR("%s", e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
