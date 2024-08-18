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

DWORD WINAPI serve_client(LPVOID arg)
{
	SOCKET* hConnected = (SOCKET*)arg;
	CSocket mysock;
	mysock.Attach(*hConnected);
	//Chuyen ve lai CSocket

	bool isConnected = true;
	

	int MsgSize;
	char* temp;
	do {
		int receive_bytes = mysock.Receive((char*)&MsgSize, sizeof(int), 0);
		if (receive_bytes <= 0) {
			isConnected = false;
			break;
		}
		else {
			temp = new char[MsgSize + 1];
			mysock.Receive(temp, MsgSize, 0);
			temp[MsgSize] = '\0';
			if (strcmp(temp, "start") == 0) {
				vector<File> files = receive_files_needed_to_send_from_client(&mysock, isConnected);
				sendFile(&mysock, files);
			}
			
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


