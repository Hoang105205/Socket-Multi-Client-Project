#include <iostream>
#include "afxsock.h"
#include "File.h"
#include "Menu.h"
#include <filesystem>
using namespace std;
namespace fs = filesystem;

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

	int MsgSize = 1;
	char Msg[100];

	for (int i = 0; i < 100; i++) {
		mysock.Receive(&MsgSize, sizeof(int), 0);
		setCursorPosition(cursor.X, cursor.Y);
		cout << MsgSize << endl;
		cursor.Y += 1;
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

		// Khoi tao Thu vien
		if (AfxSocketInit() == FALSE)
		{
			cout << "Khong the khoi tao Socket Libraray";
			return FALSE;
		}

		// Tao socket dau tien
		CSocket ClientSocket;
		ClientSocket.Create();

		// Ket noi den Server
		if (ClientSocket.Connect(_T("192.168.1.83"), 1234) != 0)
		{
			cout << "Ket noi toi Server thanh cong !!!" << endl << endl;
			if (fs::create_directory("output")) {
				cout << "Folder created successfully.\n";
			}
			else {
				cout << "Folder already exists or could not be created.\n";
			}
			signal(SIGINT, signal_callback_handler);
		/*	vector<info> files = ReceiveFiles_canbedownloaded(ClientSocket);*/
			COORD cursorPos = getCursorPosition();
			cursorPos.Y += 1;
			ifstream input;
			string input_str = "input.txt";
			input.open(input_str.c_str(), ios::app);
			input.close();
			/*vector<string> main_List = InitListIfExisted("input.txt");
			do {
				//ham cua thang Quang
				vector<string> file_needed_to_download = readNewFileAdded("input.txt", main_List, files);
				if (file_needed_to_download.size() != 0) {
					send_files_need_download_to_server(ClientSocket, file_needed_to_download);
					for (int i = 0; i < file_needed_to_download.size(); i++) {
						ReceivedFileDownload(ClientSocket, file_needed_to_download[i], cursorPos);
						cursorPos.Y += 1;
					}
				}
			} while (1);*/
			DWORD threadID;
			HANDLE threadStatus;
			SOCKET* hConnected = new SOCKET();
			//Chuyển đỏi CSocket thanh Socket
			*hConnected = ClientSocket.Detach();
			//Khoi tao thread tuong ung voi moi client Connect vao server.
			//Nhu vay moi client se doc lap nhau, khong phai cho doi tung client xu ly rieng
			COORD cursor;
			cursor.X = 5;
			cursor.Y = 10;
			ThreadParam* param = new ThreadParam;
			param->client = hConnected;
			param->cursor = cursor;
			threadStatus = CreateThread(NULL, 0, function_cal, param, 0, &threadID);


		}
		else
		{
			cout << "Khong the ket noi den Server !!!" << endl;
		}
	}
	system("pause");
	return nRetCode;
}


