#pragma once
#include "afxsock.h"
#include <vector>
#include <string>
using namespace std;
struct inputFile {
	string name;
	string priority;
};
vector<inputFile> receive_files_needed_to_send_from_client(CSocket* client, bool &isConnected);
