#include "Client.h"
#include "File.h"

vector<inputFile> receive_files_needed_to_send_from_client(CSocket* client, bool& isConnected) {
	int MsgSize;
	char* temp;
	char* name;
	char* prior;
	vector<inputFile> files;
	while (isConnected) {
		inputFile temp_inputFile;
		int receive_bytes = client->Receive((char*)&MsgSize, sizeof(int), 0);
		if (receive_bytes <= 0) {  // Client đã ngắt kết nối hoặc có lỗi
			isConnected = false;  // Đặt cờ thành false để thoát vòng lặp
			break;
		}
		temp = new char[MsgSize + 1];
		client->Receive(temp, MsgSize, 0);	
		temp[MsgSize] = '\0';
		if (strcmp(temp, "xong") != 0) {
			temp_inputFile.name = (string)temp;
			//
			cout << temp_inputFile.name << " - ";
			//
			delete[] temp;
			client->Receive((char*)&MsgSize, sizeof(int), 0);
			temp = new char[MsgSize + 1];
			client->Receive(temp, MsgSize, 0);
			temp[MsgSize] = '\0';
			temp_inputFile.priority = (string)temp;
			//
			cout << temp_inputFile.priority << endl;
			//
			delete[] temp;
			files.push_back(temp_inputFile);
		}
		else if (strcmp(temp, "xong") == 0) {
			delete[] temp;
			break;
		}
	}
	return files;
}






