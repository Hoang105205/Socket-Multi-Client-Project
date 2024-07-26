#pragma once
#include "afxsock.h"
#include <vector>
#include <string>
using namespace std;
struct inputFile {
	string name;
	string priority;
};
vector<inputFile> send_files_to_client(CSocket* client, bool &isConnected);
