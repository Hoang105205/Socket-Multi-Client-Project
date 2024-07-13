#pragma once
#include "afxsock.h"
#include <vector>
#include <string>
using namespace std;
void send_files_to_client(CSocket* client, bool& isConnected);
