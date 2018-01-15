#include <cstdlib>
#include <string>

#include <librina/librina.h>

#define RINA_PREFIX	"traffic-generator-server"
#include <librina/logs.h>

#include "tclap/CmdLine.h"

#include "config.h"
#include "test_logger.h"

using namespace std;
using namespace TCLAP;

int main(int argc, char * argv[]) {
	string	apName, apInstance, dstName, dstInstance, qos_file;
	vector<string> difs;
	vector<QoSpair> qos;
	bool log;

	try {
		CmdLine cmd("test logger", ' ', PACKAGE_VERSION);

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

		UnlabeledMultiArg<string> difs_a(
			"difs",
			"DIFs to use, empty for any DIF.",
			false,
			"string"
		);
			
		ValueArg<bool> log_a(
			"l",
			"log",
			"log, default = false.",
			false,
			false,
			"bool"
		);


		cmd.add(apName_a);
		cmd.add(apInstance_a);
		cmd.add(difs_a);
		cmd.add(log_a);
		cmd.parse(argc, argv);

		apName = apName_a.getValue();
		apInstance = apInstance_a.getValue();
		difs = difs_a.getValue();

		log = log_a.getValue();

	}
	catch (ArgException &e) {
		LOG_ERR("Error: %s for arg %d",
			e.error().c_str(),
			e.argId().c_str());
		exit(1);
	}

	try {
		rina::initialize("INFO", "");

		test_logger s(apName, apInstance, log);
		s.register_ap(difs);
		s.run();
	}
	catch (rina::Exception& e) {
		LOG_ERR("%s", e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
