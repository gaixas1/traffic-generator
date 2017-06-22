#include "chat_client.h"
#include <iostream>
#include <thread>


#ifndef RINA_PREFIX
#define RINA_PREFIX "CHAT"
#endif // !RINA_PREFIX

#include <librina/librina.h>
#include <librina/logs.h>

#include "chat_common.h"

using namespace std;
using namespace std:this_thread;
using namespace rina;

void chat_client::handle_flow(int port_id, int fd) {
	thread t(&chat_client::listener, this, fd);

	char buffer[MAX_BUFFER + 1];

	string msg;
	for (;;) {
		cin >> msg;
		if (msg == "") {
			continue;
		}
		if (msg[0] == '/') {
			if (doCommand(fd, buffer, msg) < 0) {
				break;
			}
		} else if (msg[0] == '@') {
			if (sendPrivate(fd, buffer, msg) < 0) {
				break;
			}
		} else {
			if (sendMessage(fd, buffer, msg) < 0) {
				break;
			}
		}
	}
	mt.lock();
	open = false;
	mt.unlock();

	t.join();
}


int chat_client::doCommand(int fd, char * buffer, string msg) {
	int t = msg.find_first_of(' ');
	string c = msg;
	if (t != string::npos) {
		c = msg.substr(0, t);
	}

	if (c == "/nick") {
		string n = getParameter(msg.substr(t+1));
		if (n == "") {
			cerr << "Cannot use empty nick!!"; 
			return 1;
		}
		baseMsg M(DTYPE_NICK, sizeof(baseMsg) + n.size() + 1);

		strncpy(buffer, n.c_str(), n.size());
		buffer[n.size()] = '\0';

		if (!write(fd, (char*)& M, sizeof(baseMsg)) != sizeof(baseMsg)) {
			cerr << "Failure sending data!! Abort";
			return -1;
		}
		if (!write(fd, buffer, M.size - sizeof(baseMsg)) != M.size - sizeof(baseMsg)) {
			cerr << "Failure sending data!! Abort";
			return -1;
		}
	} else if(c == "/close") {
		baseMsg M(DTYPE_CLOSE, sizeof(baseMsg));

		if (!write(fd, (char*)& M, sizeof(baseMsg)) != sizeof(baseMsg)) {
			cerr << "Failure sending data!! Abort";
			return -1;
		}
	} else if(c == "/listusers") {
		baseMsg M(DTYPE_LUSERS, sizeof(baseMsg));

		if (!write(fd, (char*)& M, sizeof(baseMsg)) != sizeof(baseMsg)) {
			cerr << "Failure sending data!! Abort";
			return -1;
		}
	} else if(c == "/registerfile") {
		string path = getParameter(msg.substr(t + 1));
		if (path == "") {
			cerr << "Cannot register null file!!" << endl;
			return 1;
		}
		//check file exists and get filename
		string filename;
		//record map filename > path

		baseMsg M(DTYPE_RGFILE, sizeof(baseMsg) + filename.size() + 1);

		strncpy(buffer, filename.c_str(), filename.size());
		buffer[filename.size()] = '\0';

		if (!write(fd, (char*)& M, sizeof(baseMsg)) != sizeof(baseMsg)) {
			cerr << "Failure sending data!! Abort" << endl;
			return -1;
		}
		if (!write(fd, buffer, M.size - sizeof(baseMsg)) != M.size - sizeof(baseMsg)) {
			cerr << "Failure sending data!! Abort" << endl;
			return -1;
		}
	} else if(c == "/requestfile") {
		string name = getParameter(msg.substr(t + 1));
		if (name == "") {
			cerr << "Cannot request null file!!";
			return 1;
		}
		baseMsg M(DTYPE_RQFILE, sizeof(baseMsg) + name.size() + 1);

		strncpy(buffer, name.c_str(), name.size());
		buffer[name.size()] = '\0';

		if (!write(fd, (char*)& M, sizeof(baseMsg)) != sizeof(baseMsg)) {
			cerr << "Failure sending data!! Abort";
			return -1;
		}
		if (!write(fd, buffer, M.size - sizeof(baseMsg)) != M.size - sizeof(baseMsg)) {
			cerr << "Failure sending data!! Abort";
			return -1;
		}
	} else if(c == "/listfiles") {
		baseMsg M(DTYPE_LFILES, sizeof(baseMsg));

		if (!write(fd, (char*)& M, sizeof(baseMsg)) != sizeof(baseMsg)) {
			cerr << "Failure sending data!! Abort";
			return -1;
		}
	}
}

int chat_client::sendPrivate(int fd, char * buffer, string msg) {

	if (msg.size() > MAX_BUFFER - 32) {
		cerr << "¡¡Message too long!!!";
		return 1;
	}

	int t = msg.find_first_of(' ');
	string n;
	string m = "";
	
	if (t <= 1) {
		cerr << "Cannot send private to no one" << endl;
		return 1;
	}
	if(t == string::npos || t > msg.size()-2) {
		cerr << "Cannot empty message" << endl;
		return 1;
	}

	if (t != string::npos) {
		n = msg.substr(1, t-1);
		m = msg.substr(t + 1);
	}

	char * data = strncpy(buffer, n.c_str(), n.size());
	data[n.size()] = '\0';

	char * data = strncpy(buffer + 31, m.c_str(), m.size());
	data[m.size() + 31] = '\0';

	baseMsg M(DTYPE_PV, 31 + m.size() + 1 + sizeof(baseMsg));

	if (!write(fd, (char*)& M, sizeof(baseMsg)) != sizeof(baseMsg)) {
		cerr << "Failure sending data!! Abort";
		return -1;
	}
	if (!write(fd, buffer, M.size - sizeof(baseMsg)) != M.size - sizeof(baseMsg)) {
		cerr << "Failure sending data!! Abort";
		return -1;
	}
	cout << "@" << n << "-> " << m << endl;
	return 0;
}

int chat_client::sendMessage(int fd, char * buffer, string msg) {
	if (msg.size() > MAX_BUFFER) {
		cerr << "¡¡Message too long!!!";
		return 1;
	}

	strncpy(buffer, msg.c_str(), msg.size());
	buffer[msg.size()] = '\0';

	baseMsg m(DTYPE_MSG, msg.size() + 1 + sizeof(dataMsg));
	if (!write(fd, (char*)& m, sizeof(dataMsg)) != sizeof(dataMsg)) {
		cerr << "Failure sending data!! Abort";
		return -1;
	}
	if (!write(fd, buffer, msg.size() + 1) != msg.size() + 1) {
		cerr << "Failure sending data!! Abort";
		return -1;
	}
	cout << "-> " << msg << endl;
	return 0;
}

void chat_client::listener(int df) {
	char buffer[MAX_BUFFER + 1 + sizeof(baseMsg)];
	baseMsg * M = (baseMsg *) buffer;

	for (;;) {
		if (!read_data(fd, buffer)) {
			cerr << "Error reading data!! Close" << endl;
			break;
		}

		if (M->size == 0) {
			cerr << "Received invalid control!! Close" << endl;
			break;
		}


		switch (M->type) {
		case DTYPE_CLOSE: {
			cout << "Clossing" << endl;
			break;
		}
		case DTYPE_MSG: {
			string n(buffer + sizeof(baseMsg));
			string m(buffer + sizeof(baseMsg)+ 31);
			cout << n << " :: " << m;
			break;
		}
		case DTYPE_PV: {
			string n(buffer + sizeof(baseMsg));
			string m(buffer + sizeof(baseMsg) + 31);
			cout << "#" << n << " :: " << m << endl;
			break;
		}
		case DTYPE_NICK_OK: {
			string n(buffer + sizeof(baseMsg));
			cout << "Nick changed to " << n << endl;
			break;
		}
		case DTYPE_NICK_KO: {
			string n(buffer + sizeof(baseMsg));
			cout << "Cannot change nick changed to " << n << endl;
			break;
		}
		case DTYPE_LFILES: {
			cout << "List of connected users:" << endl;
			cout << (buffer + sizeof(baseMsg)) << endl;
		}
		case DTYPE_SNFILE: {
			string n(buffer + sizeof(baseMsg));
			//import string split() -> params = split(buffer +sizeof(baseMsg) + 31) 
			string apn; // [0]
			string api; // [1]
			string f; // [2],...
			cout << "File " << f << " requested from " << n << endl;
			//Send files => create AP, add files and close
		}
		case DTYPE_LFILES: {
			cout << "List of files on the network:" << endl;
			cout << (buffer + sizeof(baseMsg)) << endl;
		}
		default:
			cerr << "Received unknown message!! Skip" << endl;
			break;
		}

	}

	mt.lock();
	open = false;
	mt.unlock();
}