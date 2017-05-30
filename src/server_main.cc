#include <cstdlib>
#include <string>

#include <librina/librina.h>

#define RINA_PREFIX	"traffic-generator-server"
#include <librina/logs.h>

#include "tclap/CmdLine.h"

#include "config.h"
#include "server.h"

using namespace std;
using namespace TCLAP;

int main(int argc, char * argv[]) {
	string	apName, apInstance;
	vector<string> difs;
	int		interval_duration;
	int		latency_range;

	try {
		CmdLine cmd("traffic-generator-server", ' ', PACKAGE_VERSION);

		ValueArg<string> apName_a(
			"n",
			"apName",
			"Application process name, default = traffic.generator.server.",
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

		ValueArg<int> latency_range_a(
			"L",
			"latency_range",
			"Size of latency ranges when recording in ms, default = 10ms, min 1ms.",
			false,
			10,
			"int"
		);

		cmd.add(apName_a);
		cmd.add(apInstance_a);
		cmd.add(interval_duration_a);
		cmd.add(latency_range_a);
		cmd.add(difs_a);
		cmd.parse(argc, argv);

		apName = apName_a.getValue();
		apInstance = apInstance_a.getValue();
		difs = difs_a.getValue();
		interval_duration = interval_duration_a.getValue();
		latency_range = latency_range_a.getValue();
		if (latency_range <= 0) {
			latency_range_a = 1;
		}

	} catch (ArgException &e) {
		LOG_ERR("Error: %s for arg %d",
			e.error().c_str(),
			e.argId().c_str());
		exit(1);
	}

	try {
		rina::initialize("INFO", "");

		server s(apName, apInstance);
		s.register_ap(difs);
		s.setRecordInterval(interval_duration > 0, interval_duration);
		s.setRecordRange(latency_range);
		s.run();
	} catch (rina::Exception& e) {
		LOG_ERR("%s", e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
