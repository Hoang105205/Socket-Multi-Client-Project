#include "Client.h"
#include "File.h"
void send_files_to_client(CSocket* client, bool& isConnected) {
	int MsgSize;
	vector<string> files;
	do {
		files.clear();
		while (isConnected && client->Receive((char*)&MsgSize, sizeof(int), 0)) {
			char* temp = new char[MsgSize + 1];
			int receivebytes = client->Receive((char*)temp, MsgSize, 0);
			if (receivebytes <= 0) {  // Client đã ngắt kết nối hoặc có lỗi
				isConnected = false;  // Đặt cờ thành false để thoát vòng lặp
				delete[] temp;
				break;
			}
			temp[MsgSize] = '\0';
			if (strcmp(temp, "xong") != 0) {
				files.push_back((string)temp);
				delete[] temp;
			}
			else if (strcmp(temp, "xong") == 0) {
				delete[] temp;
				break;
			}
		}
		if (!isConnected) {
			break;  // Thoát khỏi vòng lặp chính nếu client ngắt kết nối
		}
		for (int i = 0; i < files.size(); i++) {
			sendFile(files[i].c_str(), client);
		}
	} while (isConnected);
}
