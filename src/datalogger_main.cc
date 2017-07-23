#include <cstdlib>
#include <string>

#include <librina/librina.h>

#define RINA_PREFIX	"readrop"
#include <librina/logs.h>

#include "tclap/CmdLine.h"

#include "config.h"
#include "datalogger.h"

using namespace std;
using namespace TCLAP;

int main(int argc, char * argv[]) {
	string	apName, apInstance;
	vector<string> difs;
	int countToLog;

	try {
		CmdLine cmd("traffic-generator-server", ' ', PACKAGE_VERSION);

		ValueArg<string> apName_a(
			"n",
			"apName",
			"Application process name, default = logger.",
			false,
			"logger",
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
		
		ValueArg<int> countToLog_a(
			"c",
			"countToLog",
			"PDUs to count before loggin, default = 1000.",
			false,
			1000,
			"int"
		);

		cmd.add(apName_a);
		cmd.add(apInstance_a);
		cmd.add(countToLog_a);
		cmd.add(difs_a);
		cmd.parse(argc, argv);

		apName = apName_a.getValue();
		apInstance = apInstance_a.getValue();
		countToLog = countToLog_a.getValue();
		difs = difs_a.getValue();
		
	} catch (ArgException &e) {
		LOG_ERR("Error: %s for arg %d",
			e.error().c_str(),
			e.argId().c_str());
		exit(1);
	}

	try {
		rina::initialize("INFO", "");

		datalogger s(apName, apInstance, countToLog);
		s.register_ap(difs);
		s.run();
	} catch (rina::Exception& e) {
		LOG_ERR("%s", e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
