#pragma once
#include <string>
#include <vector>
#include "afxsock.h"
#include <iostream>
#include <fstream>
#include <Windows.h>
#include "File.h"
#include <ratio>
#include <iomanip>
#include <signal.h>
#include <queue>
#include <filesystem>

using namespace std;
struct info {
	string name;
	string size;
};



extern atomic<bool> offFlag;
extern mutex mtx;
extern queue<vector<inputFile>> file_download;
vector<info> ReceiveFiles_canbedownloaded(CSocket& ClientSocket);
void signal_callback_handler(int signum);
vector<string> StringToVector(string temp);
bool checkInfo(inputFile temp, vector<info> infos, string level[]);
void readNewFileAdded(string filename, vector<inputFile>& fileList, vector<info> List, string Level[]);
void HandleReceiveError(CSocket& socket);
void send_files_need_download_to_server(CSocket& client, vector<File> filename);
void receiveFile(vector<File>& files, CSocket& client, COORD current);
vector<long long> receiveFilesize(CSocket& client);
void set_up();