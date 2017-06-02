#include "filesend.h"
#include <fstream>
#include <iostream>

#include <unistd.h>
#include <condition_variable>

#ifndef RINA_PREFIX
#define RINA_PREFIX "FILESEND"
#endif // !RINA_PREFIX

#include <librina/librina.h>
#include <librina/logs.h>

using namespace std;
using namespace rina;

bool readData(const int & fd, char * buffer, int len) {
	int padding = 0;
	int ret;

	while (padding < len) {
		ret = read(fd, buffer, len - padding);
		if (ret < 0) {
			cerr << "read() failed: " << strerror(errno) << endl;
			return false;
		}
		padding += ret;
	}
	return true;
}


controlMsg::controlMsg(int _type) :
	type(_type) {}

fileInfo::fileInfo() :
	len(0) {
	filename[0] = '\0';
}

fileInfo::fileInfo(const int & _len, const string & _filename) :
	len(_len) {
	if (_filename.size() < 250) {
		strcpy(filename, _filename.c_str());
	} else {
		filename[0] = '\0';
	}
}

privFileInfo::privFileInfo(const int & _len, const string & _path, const string & _name) :
	len(_len) {
	if (_name.size() < 250) {
		name = _name;
		path = _path;
	} else {
		name = "";
		path = "";
		len = 0;
	}
}


void filesend_client::addFile(string private_path, string public_filename) {
	ifstream in(private_path, std::ifstream::ate | std::ifstream::binary);
	if (!in.is_open()) {
		cerr << "File " << private_path << " not exist or not accessible" << endl;
		return;
	}
	
	privFileInfo info(in.tellg(), private_path, public_filename);
	in.close();

	if (info.len <= 0) {
		cerr << "Public file's name " << public_filename << " too long (max 250 characters)" << endl;
		return;
	}

	mt.lock();
	remainingFiles.push_back(info);
	mt.unlock();
}

char * filesend_client::readFile(const int & len, const string & filename) {
	ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
	if (!in.is_open()) {
		cerr << "File " << filename << " not exist or not accessible" << endl;
		return nullptr;
	}

	char * raw = new char[len];
	in.read(raw, len);
	in.close();
	return raw;
}

void filesend_client::close(string private_path, string public_filename) {
	mt.lock();
	open = false;
	mt.unlock();
}

void filesend_server::setDownloadPath(string path) {
	if (path.back() != '/') {
		path += '/';
	}
	if (access(path, W_OK)) {
		downloadPath = path;
		cout << "Set downloads path as " << path << endl;;
	} else {
		cout << "Path " << path << " not valid or writable." << endl;;
	}
}

void filesend_server::setAutoAccept(bool _autoAccept) {
	autoAccept = _autoAccept;
}


void filesend_client::handle_flow(int port_id, int fd) {
	controlMsg ctr;
	fileInfo finfo;

	char * ctrBuffer = (char*)& ctr;
	char * finfoBuffer = (char*)& finfo;

	for (;;) {
		privFileInfo current;
		unique_lock<std::mutex> lck(mt);
		while (!hasFiles && open) { cv.wait(lck); }
		if (remainingFiles.empty()) {
			if (!open) {
				lck.unlock();
			} else {
				cerr << "No files but notified!! Skip" << endl;
				hasFiles = false;
				lck.unlock();
			}
			break;
		} else {
			current = remainingFiles.front();
			remainingFiles.pop();
			hasFiles = !remainingFiles.empty();
		}
		lck.unlock();

		finfo = fileInfo(current.len, current.name);
		if (finfo.len <= 0) {
			cerr << "File not valid " << current.name << "!! Skip" << endl;
			break;
		}

		if (write(fd, finfoBuffer, sizeof(fileInfo)) != sizeof(fileInfo)) {
			cerr << "write() failed: " << strerror(errno) << "!! Close" << endl;
			break;
		}

		if (!readData(fd, ctrBuffer, sizeof(controlMsg))) {
			cerr << "Error reading data!! Closing." << endl;
			break;
		}
		if (ctr.type == 3) {
			cout << "File rejected. Skip" << endl;
			break;
		} else if (ctr.type == 2) {
			cout << "File accepted. Send" << endl;
		} else {
			cerr << "Received invalid control message!! Skip";
			break;
		}
	
		char * raw = readFile(current.len, current.path);
		if (raw == nullptr) {
			cerr << "Problems reading source file " << current.path << "!! Skip" << endl;
			break;
		}

		if (write(fd, raw, current.len) != current.len) {
			cerr << "write() failed: " << strerror(errno) << "!! Close" << endl;
			break;
		}

		delete raw;

		if (!readData(fd, ctrBuffer, sizeof(controlMsg))) {
			cerr << "Error reading data!! Closing." << endl;
			break;
		}

		if (ctr.type == 4) {
			cout << "Send file ended" << endl;
			break;
		} else {
			cerr << "Received invalid control message!! Skip";
			break;
		}
	}
	release_flow(port_id);
}


void filesend_server::handle_flow(int port_id, int fd) {
	controlMsg ctr;
	fileInfo finfo;

	char * ctrBuffer = (char*)& ctr;
	char * finfoBuffer = (char*)& finfo;

	for (;;) {
		if (!readData(fd, ctrBuffer, sizeof(controlMsg))) {
			cerr << "Error reading data!! Closing." << endl;
			break;
		}
		if (ctr.type == 0) {
			break;
		} else if (ctr.type == 1) {
			if (!readData(fd, finfoBuffer, sizeof(fileInfo))) {
				cerr << "Error reading data!! Closing." << endl;
				break;
			}
			if (finfo.len <= 0) {
				cerr << "Received request for empty file!! Skip";
				ctr.type = 0;
				write(fd, ctrBuffer, sizeof(controlMsg));
			} else {
				cout << "Received request for file " << finfo.filename << endl;
				if (autoAccept) {
					cout << "\tAuto accept" << endl;
					ctr.type = 2;
					write(fd, ctrBuffer, sizeof(controlMsg));
				} else {
					cout << "\tAccept (y/n)" << endl;
					string response;
					do {
						cin >> response;
					} while (response != "y" && response != "n");
					if (response == "y") {
						cout << "\tAccepted" << endl;
						ctr.type = 2;
						write(fd, ctrBuffer, sizeof(controlMsg));
						if (!receiveFile(fd, finfo)) {
							break;
						} else {
							cout << "\tFile received" << endl;
							ctr.type = 4;
							write(fd, ctrBuffer, sizeof(controlMsg));
						}
					} else {
						cout << "\tRejected" << endl;
						ctr.type = 3;
						write(fd, ctrBuffer, sizeof(controlMsg));
					}
				}
			}

		} else {
			cerr << "Received invalid control message!! Skip";
		}
	}
}


bool receiveFile(const int & fd, const fileInfo & finfo) {
	char * raw = new char[finfo.len];

	if (!readData(fd, raw, finfo.len)) {
		cerr << "Error reading file!! Closing." << endl;
		return false;
	}

	string dstName = downloadPath + finfo.filename;

	fstream bin(dstName, ios::out | ios::binary);
	bin.write(raw, sizeof(finfo.len));
	bin.close();

	delete[] raw;
}