#include "Client.h"
#include "File.h"

vector<inputFile> receive_files_from_client(CSocket* client) {
	int MsgSize;
	char* temp;
	vector<inputFile> files;
	do {
		inputFile temp_inputFile;
		client->Receive((char*)MsgSize, sizeof(int), 0);
		temp = new char[MsgSize];
		client->Receive(temp, MsgSize, 0);
		temp[MsgSize - 1] = '\0';
		if (strcmp(temp, "xong") != 0) {
			temp_inputFile.name = (string)temp;
			delete[] temp;
			client->Receive((char*)MsgSize, sizeof(int), 0);
			temp = new char[MsgSize];
			client->Receive(temp, MsgSize, 0);
			temp[MsgSize - 1] = '\0';
			temp_inputFile.priority = (string)temp;
			delete[] temp;
			files.push_back(temp_inputFile);
		}
		else {
			delete[] temp;
			break;
		}
	} while (1);
	return files;
}

void send_files_to_client(CSocket* client, bool& isConnected) {
	int MsgSize;
	vector<inputFile> files;
	do {
		files.clear();
		while (isConnected) {
			files = receive_files_from_client(client);
		}
		if (!isConnected) {
			break;  // Thoát khỏi vòng lặp chính nếu client ngắt kết nối
		}
		
			sendFile(files[i].c_str(), client);
		
	} while (isConnected);
}
