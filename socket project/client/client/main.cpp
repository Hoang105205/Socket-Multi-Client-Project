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


vector<File> clean_list(vector<File> files) {
	//kick nhung file da tai xong
	vector<File> res;
	int index = 0;
	while (index < files.size()) {
		if (files[index].send_all_bytes != true) {
			res.push_back(files[index]);
		}					
		index++;
	}
	return res;
}

void send_start(CSocket& client) {
	const char* start = "start";
	int size = strlen(start);
	client.Send(&size, sizeof(int), 0);
	client.Send(start, size, 0);
}

void merge_list(vector<File>& files, vector<inputFile> input) {
	for (int i = 0; i < input.size(); i++) {
		bool flag = true;
		for (int j = 0; j < files.size(); j++) {
			if (input[i].name == files[j].filename) {
				flag = false;
				if (input[i].priority < files[j].priority) {
					files[j].priority = input[i].priority;
				}
				break;
			}
		}
		if (flag == true) {
			File put;
			put.filename = input[i].name;
			put.priority = input[i].priority;
			put.new_file = true;
			put.send_all_bytes = false;
			put.position = 0;
			files.push_back(put);
		}
	}
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
			

			vector<File> list;
			while (1){
				clean_list(list);
				if (!file_download.empty()) {
					vector<inputFile> input = file_download.front();
					merge_list(list, input);
					file_download.pop();
				}
				if (!list.empty()) {
					send_start(ClientSocket);
					send_files_need_download_to_server(ClientSocket, list);
					receiveFile(list, ClientSocket, getCursorPosition());
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


