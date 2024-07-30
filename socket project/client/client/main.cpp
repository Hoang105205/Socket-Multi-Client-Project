#include <iostream>
#include "afxsock.h"
#include "File.h"
#include "Menu.h"
using namespace std;

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
	const char* start = "start";
	int size = strlen(start);
	ClientSocket.Send(&size, sizeof(int), 0);
	ClientSocket.Send(start, size, 0);
	/*receiveFile(param->file, ClientSocket, param->cursor);*/
	
	delete param;
	return 0;
}

void send_start(CSocket &client) {
	const char* start = "start";
	int size = strlen(start);
	client.Send(&size, sizeof(int), 0);
	client.Send(start, size, 0);
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
		if (ClientSocket.Connect(_T("192.168.2.12"), 1234) != 0)
		{
			cout << "Ket noi toi Server thanh cong !!!" << endl << endl;
			signal(SIGINT, signal_callback_handler);
			set_up();
			vector<info> files = ReceiveFiles_canbedownloaded(ClientSocket);
			vector<inputFile> main_List = InitListIfExisted("input.txt");
			string Level[3] = { "CRITICAL", "HIGH", "NORMAL" };
			thread th(readNewFileAdded, "input.txt", ref(main_List), files, Level);
			
			while (1){
				if (!file_download.empty()) {
					vector<inputFile> files = file_download.front();
					send_start(ClientSocket);
					send_files_need_download_to_server(ClientSocket, files);
					file_download.pop();
				}
			}

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


