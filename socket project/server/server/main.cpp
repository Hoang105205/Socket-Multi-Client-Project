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
	COORD cursor;
};


DWORD WINAPI function_cal(LPVOID arg)
{
	ThreadParam* param = (ThreadParam*)arg;
	SOCKET* hConnected = param->client;
	COORD cursor = param->cursor;
	CSocket mysock;
	//Chuyen ve lai CSocket
	mysock.Attach(*hConnected);

	bool isConnected = true;
	int MsgSize = 1;
	char Msg[100];
	
	for (int i = 0; i < 100; i++) {
		mysock.Send(&i, sizeof(int), 0);
	}
		
	delete hConnected;
	delete param;
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
			//Khoi tao con tro Socket
			SOCKET* hConnected = new SOCKET();
			//Chuyển đỏi CSocket thanh Socket
			*hConnected = Connector.Detach();
			//Khoi tao thread tuong ung voi moi client Connect vao server.
			//Nhu vay moi client se doc lap nhau, khong phai cho doi tung client xu ly rieng
			COORD cursor;
			cursor.X = 5;
			cursor.Y = 10;
			ThreadParam *param = new ThreadParam;
			param->client = hConnected;
			param->cursor = cursor;
			threadStatus = CreateThread(NULL, 0, function_cal, param, 0, &threadID);
		} while (1);
		
	}
	system("pause");
	return nRetCode;
}


