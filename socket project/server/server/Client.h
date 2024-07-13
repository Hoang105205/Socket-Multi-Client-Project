#pragma once
#include "afxsock.h"
#include <vector>
#include <string>
struct inputFile {
	string name;
	string priority;
};

using namespace std;
void send_files_to_client(CSocket* client, bool& isConnected);
vector<inputFile> receive_files_from_client(CSocket* client);
