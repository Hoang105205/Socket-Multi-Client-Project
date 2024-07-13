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
using namespace std;
struct info {
	string name;
	string size;
};
extern atomic<bool> offFlag;

extern atomic<bool> pauseFlag;
extern condition_variable cv;
extern mutex mtx;

extern condition_variable cv_Download;
extern mutex mtx_Download;
extern atomic<bool> DownFlag;

void signal_callback_handler(int signum);
vector<string> StringToVector(string temp);
bool checkInfo(string temp, vector<info> infos);
vector<info> ReceiveFiles_canbedownloaded(CSocket& ClientSocket);
void ReceivedFileDownload(CSocket& ClientSocket, string filename, COORD cursorPos);
vector<string> readNewFileAdded(string filename, vector<string>& fileList, vector<info> List);
void send_files_need_download_to_server(CSocket& client, vector<string> filename);
void resumeThread();
