#include <cstdlib>
#include <string>

#include <librina/librina.h>

#define RINA_PREFIX	"videologger"
#include <librina/logs.h>

#include "tclap/CmdLine.h"

#include "config.h"
#include "videologger.h"

using namespace std;
using namespace TCLAP;

int main(int argc, char * argv[]) {
	string	apName, apInstance;
	vector<string> difs;
	int minBuffer, maxBuffer;
	double playHz;

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
		
		ValueArg<int> minBuffer_a(
			"b",
			"minBuffer",
			"Min PDUs to buffer, default = 10.",
			false,
			10,
			"int"
		);
		ValueArg<int> maxBuffer_a(
			"B",
			"maxBuffer",
			"Max PDUs to buffer, default = 1000.",
			false,
			1000,
			"int"
		);

		ValueArg<double> playHz_a(
			"H",
			"playHz",
			"Speed of video playback in PDUs/s, default = 30.",
			false,
			50.0,
			"double"
		);

		cmd.add(apName_a);
		cmd.add(apInstance_a);
		cmd.add(minBuffer_a);
		cmd.add(maxBuffer_a);
		cmd.add(playHz_a);
		cmd.add(difs_a);
		cmd.parse(argc, argv);

		apName = apName_a.getValue();
		apInstance = apInstance_a.getValue();
		minBuffer = minBuffer_a.getValue();
		maxBuffer = maxBuffer_a.getValue();
		playHz = playHz_a.getValue();
		difs = difs_a.getValue();
		
	} catch (ArgException &e) {
		LOG_ERR("Error: %s for arg %d",
			e.error().c_str(),
			e.argId().c_str());
		exit(1);
	}

	try {
		rina::initialize("INFO", "");

		videologger s(apName, apInstance, minBuffer, maxBuffer, playHz);
		s.register_ap(difs);
		s.run();
	} catch (rina::Exception& e) {
		LOG_ERR("%s", e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
