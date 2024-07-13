#pragma once
#include "afxsock.h"
#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include "Client.h"
using namespace std;
struct info {
	string name;
	string size;
};
vector<info> readInfo(string filename);
void SendListFile(CSocket* Connector, string filename);
void sendFile(const char* filename, CSocket* Connector, vector<inputFile> files);
