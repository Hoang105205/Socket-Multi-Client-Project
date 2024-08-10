#pragma once
#include "afxsock.h"
#include <vector>
#include <string>
#include "File.h"
using namespace std;
struct inputFile {
	string name;
	string priority;
};

struct File {
	long long position = 0;
	string filename = "";
	string priority = "";
	bool send_all_bytes = false;
};

vector<File> receive_files_needed_to_send_from_client(CSocket* client, bool& isConnected);
void sendFilesize(CSocket* client, vector<File> files);
void sendFile(CSocket* client, vector<File> files);
