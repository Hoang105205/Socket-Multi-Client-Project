#include <iostream>
#include "afxsock.h"
#include "File.h"
#include "Menu.h"
#include <filesystem>
using namespace std;
namespace fs = filesystem;

void set_up() {
	if (fs::create_directory("output")) {
		cout << "Folder created successfully.\n";
	}
	else {
		cout << "Folder already exists or could not be created.\n";
	}
	ifstream input;
	string input_str = "input.txt";
	input.open(input_str.c_str(), ios::app);
	input.close();
}

HANDLE Mutex;

struct ThreadParam{
	SOCKET* client;
	vector<inputFile> file;
	COORD cursor;
};


DWORD WINAPI function_cal(LPVOID arg)
{
	ThreadParam* param = (ThreadParam*)arg;
	SOCKET* client = param->client;
	CSocket ClientSocket;
	ClientSocket.Attach(*client);
	send_files_need_download_to_server(ClientSocket, param->file);
	receiveFile(param->file, ClientSocket, param->cursor);
	delete client;
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
			signal(SIGINT, signal_callback_handler);
		
			COORD cursorPos = getCursorPosition();
			cursorPos.Y += 1;

			vector<info> files = ReceiveFiles_canbedownloaded(ClientSocket);
			vector<inputFile> main_List = InitListIfExisted("input.txt");
			string Level[3] = { "CRITICAL", "HIGH", "NORMAL" };
			thread th(readNewFileAdded, "input.txt", ref(main_List), files, Level);
			Mutex = CreateMutex(NULL, FALSE, NULL);

			do {
				if (file_download.size() != 0) {
					WaitForSingleObject(Mutex, INFINITE);
					DWORD threadID;
					HANDLE threadStatus;
					SOCKET* hConnected = new SOCKET();
					*hConnected = ClientSocket.Detach();
					ThreadParam* param = new ThreadParam();
					param->client = hConnected;
					param->file = file_download.front();
					param->cursor = getCursorPosition();
					threadStatus = CreateThread(NULL, 0, function_cal, param, 0, &threadID);
					file_download.pop();
					ReleaseMutex(Mutex);
				}
			} while (1);

			CloseHandle(Mutex);
			offFlag = true;
			th.detach();

			ClientSocket.Close();
		}
		else
		{
			cout << "Khong the ket noi den Server !!!" << endl;
		}
	}
	system("pause");
	return nRetCode;
}


