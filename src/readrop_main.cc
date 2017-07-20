#include <cstdlib>
#include <string>

#include <librina/librina.h>

#define RINA_PREFIX	"readrop"
#include <librina/logs.h>

#include "tclap/CmdLine.h"

#include "config.h"
#include "readrop.h"

using namespace std;
using namespace TCLAP;

int main(int argc, char * argv[]) {
	string	apName, apInstance;
	vector<string> difs;

	try {
		CmdLine cmd("traffic-generator-server", ' ', PACKAGE_VERSION);

		ValueArg<string> apName_a(
			"n",
			"apName",
			"Application process name, default = readrop.",
			false,
			"readrop",
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

		cmd.add(apName_a);
		cmd.add(apInstance_a);
		cmd.add(difs_a);
		cmd.parse(argc, argv);

		apName = apName_a.getValue();
		apInstance = apInstance_a.getValue();
		difs = difs_a.getValue();
		
	} catch (ArgException &e) {
		LOG_ERR("Error: %s for arg %d",
			e.error().c_str(),
			e.argId().c_str());
		exit(1);
	}

	try {
		rina::initialize("INFO", "");

		readrop s(apName, apInstance);
		s.register_ap(difs);
		s.run();
	} catch (rina::Exception& e) {
		LOG_ERR("%s", e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
