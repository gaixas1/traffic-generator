#pragma once
#include "simple_client.h"
#include "simple_server.h"
#include "mutex"
#include <queue>

using namespace std;

struct controlMsg {
	int type;		// 0-close, 1-fileinfo, 2-okFile, 3-rejectFile, 4-fileReceived
	controlMsg(int _type = 0);
};

struct fileInfo {
	int len;
	char filename[255];
	fileInfo();
	fileInfo(const int & _len, const string & _filename);
};

struct privFileInfo {
	int len;
	string path, name;
	privFileInfo();
	privFileInfo(const int & _len, const string & _path, const string & _name);
};

class filesend_client : public simple_client {
public:
	filesend_client(const string & apn, const string & api, const string & _dst_apn, const string & _dst_api, const vector<QoSpair> &_qos)
		: simple_client(apn, api, _dst_apn, _dst_api, _qos), open(true), hasFiles(false) {}

	void addFile(string private_path, string public_filename);
	void close(string private_path, string public_filename);
	void handle_flow(int port_id, int fd);

protected:
	mutex mt;
	condition_variable cv;
	bool hasFiles, open;
	queue<privFileInfo> remainingFiles;

	char * readFile(const int & len, const string & filename);
};

class filesend_server : public simple_server {
public:
	filesend_server(const string & apn, const string & api)
		: simple_server(apn, api), autoAccept(false), downloadPath("./"){};

	void setAutoAccept(bool _autoAccept);
	void setDownloadPath(string path);
	void handle_flow(int port_id, int fd);

protected :
	bool autoAccept;
	mutex mt;
	string downloadPath;

	bool receiveFile(const int & fd, const fileInfo & finfo);
};
