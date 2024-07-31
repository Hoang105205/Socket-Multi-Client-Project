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




void sort_list_files(vector<File> &files) {
	//kick nhung file da tai xong
	for (int i = files.size() - 1; i >= 0; i--) {
		if (files[i].send_all_bytes == true) {
			if (i != files.size() - 1) {
				for (int k = i; k < files.size() - 1; k++) {
					swap(files[k], files[k + 1]);
				}
			}
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

void merge_list(vector<File>& files, vector<inputFile> input) {
	for (int i = 0; i < input.size(); i++) {
		File put;
		put.filename = input[i].name;
		put.priority = input[i].priority;
		put.new_file = true;
		put.send_all_bytes = false;
		put.position = 0;
		files.push_back(put);
	}
	sort_list_files(files);
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
			/*string Level[3] = { "CRITICAL", "HIGH", "NORMAL" };
			thread th(readNewFileAdded, "input.txt", ref(main_List), files, Level);*/
			

			vector<File> list;
			/*while (1){
				if (!file_download.empty()) {
					vector<inputFile> input = file_download.front();
					merge_list(list, input);
					send_start(ClientSocket);
					send_files_need_download_to_server(ClientSocket, list);
					receiveFile(list, ClientSocket, getCursorPosition());
					file_download.pop();
				}
			}*/
			File a;
			a.filename = "50MB.zip";
			a.new_file = true;
			a.position = 0;
			a.priority = "CRITICAL";
			a.send_all_bytes = false;
			list.push_back(a);
			vector<inputFile> b;
			while (!list.empty()) {
				send_files_need_download_to_server(ClientSocket, list);
				receiveFile(list, ClientSocket, getCursorPosition());

				merge_list(list, b);
			}

			/*offFlag = true;
			th.detach();*/

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


