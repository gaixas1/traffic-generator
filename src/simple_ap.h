/*
 * Base class for simple RINA applications
 *
 *   Addy Bombeke	   <addy.bombeke@ugent.be>
 *   Dimitri Staessens	   <dimitri.staessens@intec.ugent.be>
 *   Douwe De Bock	   <douwe.debock@ugent.be>
 *   Francesco Salvestrini <f.salvestrini@nextworks.it>
 *
 *   Edit by Sergio Leon <slgaixas@ac.upc.edu>
 *
 * This source code has been released under the GEANT outward license.
 * Refer to the accompanying LICENSE file for further information
 */

#pragma once

#include <string>
#include <vector>
#include <map>

using namespace std;

struct port_fd {
	int port, fd;
	port_fd(int _port, int _fd) : port(_port), fd(_fd) {}
};

struct QoSpair {
	string param;
	string value;
	QoSpair(string _param, string _value)
		:param(_param), value(_value) {}
};

bool parseQoSRequirements(vector<QoSpair> & qos, string filename);

class simple_ap {
public:
	simple_ap(const string& apn) :name (apn), instance ("1") {}
	simple_ap(const string& apn, const string& api) : name (apn), instance (api) {}

	virtual ~simple_ap();

	bool register_ap();
	bool register_ap(const string& dif_name);
	bool register_ap(const vector<string>& dif_names);
	bool unregister_ap();
	bool unregister_ap(const string& dif_name);
	bool unregister_ap(const vector<string>& dif_names);

	port_fd request_flow(const string& apn, const string& api, const vector<QoSpair> & rq);
	port_fd request_flow(const string& apn, const string& api, const vector<QoSpair> & rq, const string& dif_name);

	int release_flow(const int port_id);
	int release_all_flows();

protected:
	string name;
	string instance;
	vector<string> reg_difs;
	map<int,string> my_flows;
};
