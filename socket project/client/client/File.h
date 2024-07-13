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
void writeInfo(char infos[]);
void writeFile(vector<string> file, string filename, COORD cursorPos);
bool isFileEmpty(string filename);
void setCursorPosition(int x, int y);
COORD getCursorPosition();
vector<string> InitListIfExisted(string filename);