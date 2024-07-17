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
using namespace std;
struct inputFile {
	string name;
	string priority;
};

void writeInfo(char infos[]);
bool isFileEmpty(string filename);
void setCursorPosition(int x, int y);
COORD getCursorPosition();
vector<string> InitListIfExisted(string filename);