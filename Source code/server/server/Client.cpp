#include "Client.h"
#include "File.h"

vector<File> receive_files_needed_to_send_from_client(CSocket* client, bool& isConnected) {
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

void sendFile(CSocket* client, vector<File> files)
{
	int MsgSize = 1048576;
	long long MsgSize_temp = 1048576;
	char* temp;
	int bytes_left;
	sendFilesize(client, files);
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

				char* received_flag;
				int received_size;
				client->Receive((char*)&received_size, sizeof(received_size), 0);
				received_flag = new char[received_size + 1];
				client->Receive(received_flag, received_size, 0);
				received_flag[received_size] = '\0';
				if (strcmp(received_flag, "nhan het") == 0)
				{
					const char* flag = "done";
					int size_flag = strlen(flag);
					client->Send(&size_flag, sizeof(size_flag), 0);
					client->Send(flag, size_flag, 0);
					files[i].send_all_bytes = true;
				}
				delete[] received_flag;
				files[i].send_all_bytes = true;
				delete[] temp;

			}
			else {
				if (files[i].priority == "CRITICAL") {
					for (int j = 0; j < 10; j++) {
						if (cur_pos + MsgSize_temp >= file_size) {
							bytes_left = (int)(file_size - cur_pos);
							fin.seekg(cur_pos, ios::beg);
							temp = new char[bytes_left];
							fin.read(temp, bytes_left);

							client->Send(&bytes_left, sizeof(bytes_left), 0);
							client->Send(temp, bytes_left, 0);

							char* received_flag;
							int received_size;
							client->Receive((char*)&received_size, sizeof(received_size), 0);
							received_flag = new char[received_size + 1];
							client->Receive(received_flag, received_size, 0);
							received_flag[received_size] = '\0';
							if (strcmp(received_flag, "nhan het") == 0)
							{
								const char* flag = "done";
								int size_flag = strlen(flag);
								client->Send(&size_flag, sizeof(size_flag), 0);
								client->Send(flag, size_flag, 0);
								files[i].send_all_bytes = true;
							}
							delete[] received_flag;

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

							char* received_flag;
							int received_size;
							client->Receive((char*)&received_size, sizeof(received_size), 0);
							received_flag = new char[received_size + 1];
							client->Receive(received_flag, received_size, 0);
							received_flag[received_size] = '\0';
							if (strcmp(received_flag, "nhan het") == 0)
							{
								const char* flag = "undone";
								int size_flag = strlen(flag);
								client->Send(&size_flag, sizeof(size_flag), 0);
								client->Send(flag, size_flag, 0);
								files[i].send_all_bytes = true;
							}
							delete[] received_flag;

							cur_pos += MsgSize;
							delete[] temp;
						}
					}
				}
				else if (files[i].priority == "HIGH") {
					for (int k = 0; k < 4; k++) {
						if (cur_pos + MsgSize_temp >= file_size) {
							bytes_left = (int)(file_size - cur_pos);
							fin.seekg(cur_pos, ios::beg);
							temp = new char[bytes_left];
							fin.read(temp, bytes_left);

							client->Send(&bytes_left, sizeof(bytes_left), 0);
							client->Send(temp, bytes_left, 0);

							char* received_flag;
							int received_size;
							client->Receive((char*)&received_size, sizeof(received_size), 0);
							received_flag = new char[received_size + 1];
							client->Receive(received_flag, received_size, 0);
							received_flag[received_size] = '\0';
							if (strcmp(received_flag, "nhan het") == 0)
							{
								const char* flag = "done";
								int size_flag = strlen(flag);
								client->Send(&size_flag, sizeof(size_flag), 0);
								client->Send(flag, size_flag, 0);
								files[i].send_all_bytes = true;
							}
							delete[] received_flag;

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

							char* received_flag;
							int received_size;
							client->Receive((char*)&received_size, sizeof(received_size), 0);
							received_flag = new char[received_size + 1];
							client->Receive(received_flag, received_size, 0);
							received_flag[received_size] = '\0';
							if (strcmp(received_flag, "nhan het") == 0)
							{
								const char* flag = "undone";
								int size_flag = strlen(flag);
								client->Send(&size_flag, sizeof(size_flag), 0);
								client->Send(flag, size_flag, 0);
								files[i].send_all_bytes = true;
							}
							delete[] received_flag;
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

						char* received_flag;
						int received_size;
						client->Receive((char*)&received_size, sizeof(received_size), 0);
						received_flag = new char[received_size + 1];
						client->Receive(received_flag, received_size, 0);
						received_flag[received_size] = '\0';
						if (strcmp(received_flag, "nhan het") == 0)
						{
							const char* flag = "done";
							int size_flag = strlen(flag);
							client->Send(&size_flag, sizeof(size_flag), 0);
							client->Send(flag, size_flag, 0);
							files[i].send_all_bytes = true;
						}
						delete[] received_flag;
						delete[] temp;
					}
					else {
						fin.seekg(cur_pos, ios::beg);
						temp = new char[MsgSize];
						fin.read(temp, MsgSize);

						client->Send(&MsgSize, sizeof(MsgSize), 0);
						client->Send(temp, MsgSize, 0);
						char* received_flag;
						int received_size;
						client->Receive((char*)&received_size, sizeof(received_size), 0);
						received_flag = new char[received_size + 1];
						client->Receive(received_flag, received_size, 0);
						received_flag[received_size] = '\0';
						if (strcmp(received_flag, "nhan het") == 0)
						{
							const char* flag = "undone";
							int size_flag = strlen(flag);
							client->Send(&size_flag, sizeof(size_flag), 0);
							client->Send(flag, size_flag, 0);
							files[i].send_all_bytes = true;
						}
						delete[] received_flag;
						delete[] temp;
					}
				}
			}
		}
		fin.close();
	}
}





