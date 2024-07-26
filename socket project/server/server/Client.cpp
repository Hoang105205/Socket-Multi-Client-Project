#include "Client.h"
#include "File.h"

vector<inputFile> send_files_to_client(CSocket* client, bool& isConnected) {
	int MsgSize;
	char* temp;
	vector<inputFile> files;
	while (isConnected) {
		inputFile temp_inputFile;
		client->Receive((char*)&MsgSize, sizeof(int), 0);
		temp = new char[MsgSize];
		int receive_bytes = client->Receive(temp, MsgSize, 0);
		if (receive_bytes <= 0) {  // Client đã ngắt kết nối hoặc có lỗi
			isConnected = false;  // Đặt cờ thành false để thoát vòng lặp
			delete[] temp;
			break;
		}
		temp[MsgSize - 1] = '\0';
		if (strcmp(temp, "xong") != 0) {
			temp_inputFile.name = (string)temp;
			//
			cout << temp_inputFile.name << " - ";
			//
			delete[] temp;
			client->Receive((char*)&MsgSize, sizeof(int), 0);
			temp = new char[MsgSize];
			client->Receive(temp, MsgSize, 0);
			temp[MsgSize - 1] = '\0';
			temp_inputFile.priority = (string)temp;
			//
			cout << temp_inputFile.priority << endl;
			//
			delete[] temp;
			files.push_back(temp_inputFile);
		}
		else {
			delete[] temp;
			break;
		}
	}
	return files;
}






