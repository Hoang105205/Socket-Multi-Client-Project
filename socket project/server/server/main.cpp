#include "afxsock.h"
#include <iostream>
#include <cstring>
#include <vector>
#include <fstream>
#include <string>
#include <thread>
#include "Client.h"
#include "File.h"
using namespace std;

struct ThreadParam {
	SOCKET* client;
	vector<inputFile> file;
};

DWORD WINAPI send_file(LPVOID arg)
{
	ThreadParam* param = (ThreadParam*)arg;
	SOCKET* hConnected = param->client;
	vector<inputFile> files = param->file;
	CSocket mysock;
	//Chuyen ve lai CSocket
	mysock.Attach(*hConnected);
	sendFile(&mysock, files);
	delete hConnected;
	delete param;
	return 0;
}


DWORD WINAPI serve_client(LPVOID arg)
{
	SOCKET* hConnected = (SOCKET*)arg;
	CSocket mysock;
	//Chuyen ve lai CSocket
	mysock.Attach(*hConnected);

	bool isConnected = true;
	vector<inputFile> files;
	do {
		files.clear();
		files = send_files_to_client(&mysock, isConnected);
		if (isConnected == false) {
			break;
		}
		else {
			DWORD threadID;
			HANDLE threadStatus;
			ThreadParam* param = new ThreadParam();
			param->client = hConnected;
			param->file = files;
			threadStatus = CreateThread(NULL, 0, send_file, param, 0, &threadID);
		}
	} while (isConnected);		
	delete hConnected;
	return 0;
}

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: MFC initialization failed\n"));
		nRetCode = 1;
	}
	else
	{
		// TODO: code your application's behavior here.

		// Khoi tao thu vien Socket
		if (AfxSocketInit() == FALSE)
		{
			cout << "Khong the khoi tao Socket Libraray";
			return FALSE;
		}

		CSocket ServerSocket; //cha
		// Tao socket cho server, dang ky port la 1234, giao thuc TCP
		if (ServerSocket.Create(1234, SOCK_STREAM, NULL) == 0) //SOCK_STREAM or SOCK_DGRAM.
		{
			cout << "Khoi tao that bai !!!" << endl;
			cout << ServerSocket.GetLastError();
			return FALSE;
		}
		DWORD threadID;
		HANDLE threadStatus;
		cout << "Server khoi tao thanh cong !!!" << endl;
		CSocket Connector;
		do {
			cout << "Server lang nghe ket noi tu client\n";
			ServerSocket.Listen();
			ServerSocket.Accept(Connector);
			SendListFile(&Connector, "info.txt");
			//Khoi tao con tro Socket
			SOCKET* hConnected = new SOCKET();
			//Chuyển đỏi CSocket thanh Socket
			*hConnected = Connector.Detach();
			//Khoi tao thread tuong ung voi moi client Connect vao server.
			//Nhu vay moi client se doc lap nhau, khong phai cho doi tung client xu ly rieng
			threadStatus = CreateThread(NULL, 0, serve_client, hConnected, 0, &threadID);
		} while (1);
	}
	system("pause");
	return nRetCode;
}


