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


struct File {
	bool new_file = true;
	string filename = "";
	string priority = "";
	bool send_all_bytes = false;
};

void sort_list_files(vector<File> &files) {
	//kick nhung file da tai xong
	for (int i = files.size() - 1; i >= 0; i--) {
		if (files[i].send_all_bytes == true) {
			files.pop_back();
		}
	}


	for (int i = 0; i < files.size() - 1; i++) {
		for (int j = i + 1; j < files.size(); j++) {
			if (files[i].filename == files[j].filename) {
				if (files[i].priority < files[j].priority) {
					if (j != files.size() - 1) {
						//day phan tu j xuong cuoi vector
						for (int k = j; k < files.size() - 1; k++) {
							swap(files[k], files[k + 1]);
						}
					}
					files.pop_back();
				}
				else if (files[i].priority >= files[j].priority) {
					if (files[i].new_file != true) {
						files[j].new_file = files[i].new_file;
					}
					swap(files[i], files[j]);
					if (j != files.size() - 1) {
						//day phan tu j xuong cuoi vector
						for (int k = j; k < files.size() - 1; k++) {
							swap(files[k], files[k + 1]);
						}
					}
					files.pop_back();
				}
			}
		}
	}
}

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
		if (ClientSocket.Connect(_T("192.168.1.84"), 1234) != 0)
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


