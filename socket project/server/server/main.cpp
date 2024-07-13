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

void ClientHandler(CSocket* clientSocket)
{
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		cout << "Socket initialization failed in thread!" << endl;
		return;
	}

	char buffer[1024];
	int bytesReceived = 0;
	int MsgSize;
	while (true)
	{
		clientSocket->Receive((char*)&MsgSize, sizeof(int), 0);
		bytesReceived = clientSocket->Receive(buffer, MsgSize, 0);
		if (bytesReceived == SOCKET_ERROR || bytesReceived == 0)
		{
			break;
		}
		buffer[bytesReceived] = '\0';
		if (strcmp(buffer, "receive") == 0) {
			SendListFile(clientSocket, "info,txt");
			break;
		}		
	}
	clientSocket->Close();
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
		else
		{
			if (ServerSocket.Listen(1) == FALSE)
			{
				cout << "Khong the lang nghe tren port nay !!!" << endl;
				ServerSocket.Close();
				return FALSE;
			}
		}
		cout << "Server khoi tao thanh cong !!!" << endl;
		cout << "Nhap so luong client cho phep truy cap: ";
		int num_client;
		cin >> num_client;
		CSocket* Connector = new CSocket[num_client];
		for (int i = 0; i < num_client; i++)
		{
			if (ServerSocket.Accept(Connector[i]))
			{
				cout << "Client thu " << i + 1 << "ket noi thanh cong !!!" << endl;
			}
		}
		for (int i = 0; i < num_client; i++)
		{
			thread t1(ClientHandler, &Connector[i]);
			t1.detach();
		}
		delete[] Connector;
		ServerSocket.Close();
	}
	system("pause");
	return nRetCode;
}