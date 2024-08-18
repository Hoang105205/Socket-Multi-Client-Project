#pragma once
#include <iostream>
#include <chrono>
#include <thread>
#include <atomic>
#include <string>
#include <vector>
#include "afxsock.h"
#include <fstream>
#include <Windows.h>
#include <condition_variable>
#include <sstream>
using namespace std;
struct inputFile {
	string name = "";
	string priority = "";
};

struct File {
	bool new_file = true;
	string filename = "";
	string priority = "";
	bool send_all_bytes = false;
	long long position = 0;
	COORD download_cursor;
};

bool isFileEmpty(string filename);
void setCursorPosition(int x, int y);
COORD getCursorPosition();
vector<inputFile> InitListIfExisted(string filename);
vector<File> clean_list(vector<File> files);
void merge_list(vector<File>& files, vector<inputFile> input, COORD& start);