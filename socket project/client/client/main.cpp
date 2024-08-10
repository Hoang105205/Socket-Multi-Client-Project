#include <iostream>
#include "afxsock.h"
#include "File.h"
#include "Menu.h"
using namespace std;

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
			COORD start = getCursorPosition();
			start.Y += 2;
			while (1){
				list = clean_list(list);
				if (!file_download.empty()) {
					vector<inputFile> input = file_download.front();
					merge_list(list, input, start);
					file_download.pop();
				}
				if (list.size() != 0) {
					send_start(ClientSocket);
					send_files_need_download_to_server(ClientSocket, list);
					receiveFile(list, ClientSocket);
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


