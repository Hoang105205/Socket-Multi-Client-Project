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
	vector<inputFile> file;
};

struct File {
	long long position = 0;
	string filename = "";
	string priority = "";
	bool send_all_bytes = false;
};

vector<File> receive_files_needed_to_send_from_client_2(CSocket* client, bool& isConnected) {
	int MsgSize;
	char* temp_msg;
	vector<File> files;
	while (isConnected) {
		File temp_inputFile;
		int receive_bytes = client->Receive((char*)&MsgSize, sizeof(int), 0);
		if (receive_bytes <= 0) {  // Client đã ngắt kết nối hoặc có lỗi
			isConnected = false;  // Đặt cờ thành false để thoát vòng lặp
			break;
		}
		temp_msg = new char[MsgSize + 1];
		client->Receive(temp_msg, MsgSize, 0);
		temp_msg[MsgSize] = '\0';
		if (strcmp(temp_msg, "xong") != 0) {
			temp_inputFile.filename = (string)temp_msg;
			delete[] temp_msg;
			client->Receive((char*)&MsgSize, sizeof(int), 0);
			temp_msg = new char[MsgSize + 1];
			client->Receive(temp_msg, MsgSize, 0);
			temp_msg[MsgSize] = '\0';
			temp_inputFile.position = stoll((string)temp_msg);
			delete[] temp_msg;
			client->Receive((char*)&MsgSize, sizeof(int), 0);
			temp_msg = new char[MsgSize + 1];
			client->Receive(temp_msg, MsgSize, 0);
			temp_msg[MsgSize] = '\0';
			temp_inputFile.priority = (string)temp_msg;
			delete[] temp_msg;
			files.push_back(temp_inputFile);
		}
		else if (strcmp(temp_msg, "xong") == 0) {
			delete[] temp_msg;
			break;
		}
	}
	return files;
}

void sendFilesize(CSocket* client, vector<File> files)
{
	ifstream f;
	long long size;
	string temp;
	int MsgSize;
	for (int i = 0; i < files.size(); i++)
	{
		f.open(files[i].filename.c_str(), ios::binary);
		f.seekg(0, ios::end);
		size = f.tellg();
		temp = to_string(size);
		MsgSize = temp.length();
		client->Send(&MsgSize, sizeof(int), 0);
		client->Send(temp.c_str(), MsgSize, 0);
		f.close();
	}
	const char* stopflag = "completed";
	MsgSize = strlen(stopflag);
	client->Send(&MsgSize, sizeof(int), 0);
	client->Send(stopflag, MsgSize, 0);
}



long long get_file_size(ifstream &ifstream_filename) {
	ifstream_filename.seekg(0, ios::end);
	long long a = ifstream_filename.tellg();
	return a;
}

void sendFile(CSocket* client, vector<File> &files)
{
	int MsgSize = 102400;
	long long MsgSize_temp = 102400;
	char* temp;
	int bytes_left;
	sendFilesize(client, files);

	cout << files[0].position << endl;
	for (int i = 0; i < files.size(); i++) {
		ifstream fin;
		fin.open(files[i].filename.c_str(), ios::binary);
		if (fin) {
			long long file_size = get_file_size(fin);
			long long cur_pos = files[i].position;
			
		if (file_size - files[i].position < MsgSize_temp) {
				bytes_left = (int)(file_size - files[i].position);
				fin.seekg(cur_pos, ios::beg);
				temp = new char[bytes_left];
				fin.read(temp, bytes_left);		
				client->Send(&bytes_left, sizeof(bytes_left), 0);
				client->Send(temp, bytes_left, 0);

				const char* flag = "done";
				int size_flag = strlen(flag);
				client->Send(&size_flag, sizeof(size_flag), 0);
				client->Send(flag, size_flag, 0);

				files[i].send_all_bytes = true;
				delete[] temp;

			}
			else {
				if (files[i].priority == "CRITICAL") {
					for (int i = 0; i < 10; i++) {
						if (cur_pos + MsgSize_temp >= file_size) {
							bytes_left = (int)(file_size - cur_pos);
							fin.seekg(cur_pos, ios::beg);
							temp = new char[bytes_left];
							fin.read(temp, bytes_left);

							client->Send(&bytes_left, sizeof(bytes_left), 0);
							client->Send(temp, bytes_left, 0);

							const char* flag = "done";
							int size_flag = strlen(flag);
							client->Send(&size_flag, sizeof(size_flag), 0);
							client->Send(flag, size_flag, 0);

							files[i].send_all_bytes = true;
							cur_pos += MsgSize;
							delete[] temp;
							break;
						}
						else {
							fin.seekg(cur_pos, ios::beg);
							temp = new char[MsgSize];
							fin.read(temp, MsgSize);
				
							client->Send(&MsgSize, sizeof(MsgSize), 0);
							client->Send(temp, MsgSize, 0);

							const char* flag = "undone";
							int size_flag = strlen(flag);
							client->Send(&size_flag, sizeof(size_flag), 0);
							client->Send(flag, size_flag, 0);

							cur_pos += MsgSize;
							delete[] temp;
						}
					}
				}
				else if (files[i].priority == "HIGH") {
					for (int i = 0; i < 4; i++) {
						if (cur_pos + MsgSize_temp >= file_size) {
							bytes_left = (int)(file_size - cur_pos);
							fin.seekg(cur_pos, ios::beg);
							temp = new char[bytes_left];
							fin.read(temp, bytes_left);

							client->Send(&bytes_left, sizeof(bytes_left), 0);
							client->Send(temp, bytes_left, 0);

							const char* flag = "done";
							int size_flag = strlen(flag);
							client->Send(&size_flag, sizeof(size_flag), 0);
							client->Send(flag, size_flag, 0);

							files[i].send_all_bytes = true;
							cur_pos += MsgSize;
							delete[] temp;
							break;
						}
						else {
							fin.seekg(cur_pos, ios::beg);
							temp = new char[MsgSize];
							fin.read(temp, MsgSize);

							client->Send(&MsgSize, sizeof(MsgSize), 0);
							client->Send(temp, MsgSize, 0);

							const char* flag = "undone";
							int size_flag = strlen(flag);
							client->Send(&size_flag, sizeof(size_flag), 0);
							client->Send(flag, size_flag, 0);

							cur_pos += MsgSize;
							delete[] temp;
						}
					}
				}
				else if (files[i].priority == "NORMAL") {
					if (cur_pos + MsgSize_temp >= file_size) {
						bytes_left = (int)(file_size - cur_pos);
						fin.seekg(cur_pos, ios::beg);
						temp = new char[bytes_left];
						fin.read(temp, bytes_left);

						client->Send(&bytes_left, sizeof(bytes_left), 0);
						client->Send(temp, bytes_left, 0);

						const char* flag = "done";
						int size_flag = strlen(flag);
						client->Send(&size_flag, sizeof(size_flag), 0);
						client->Send(flag, size_flag, 0);

						files[i].send_all_bytes = true;
						delete[] temp;
					}
					else {
						fin.seekg(cur_pos, ios::beg);
						temp = new char[MsgSize];
						fin.read(temp, MsgSize);

						client->Send(&MsgSize, sizeof(MsgSize), 0);
						client->Send(temp, MsgSize, 0);

						const char* flag = "undone";
						int size_flag = strlen(flag);
						client->Send(&size_flag, sizeof(size_flag), 0);
						client->Send(flag, size_flag, 0);

						delete[] temp;
					}
				}
			}
		}
		fin.close();
	}
}




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
				vector<File> files = receive_files_needed_to_send_from_client_2(&mysock, isConnected);
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


