#include "File.h"
using namespace std;

vector<info> readInfo(string filename) {
	ifstream f;
	vector<info> infos;
	f.open(filename.c_str());
	if (f) {
		info temp;
		while (!f.eof()) {
			getline(f, temp.name, ' ');
			getline(f, temp.size, '\n');
			infos.push_back(temp);
		}
	}
	f.close();
	return infos;
}

void SendListFile(CSocket* Connector, string filename)
{
	vector<info> infos = readInfo(filename.c_str());

	int MsgSize;

	int size_num = infos.size();
	Connector->Send(&size_num, sizeof(size_num), 0);
	Connector->Send((char*)size_num, size_num, 0);

	for (int i = 0; i < infos.size(); i++) {
		MsgSize = infos[i].name.length();
		Connector->Send(&MsgSize, sizeof(MsgSize), 0);
		Connector->Send(infos[i].name.c_str(), MsgSize, 0);

		MsgSize = infos[i].size.length();
		Connector->Send(&MsgSize, sizeof(MsgSize), 0);
		Connector->Send(infos[i].size.c_str(), MsgSize, 0);
	}
}

void sendFile(const char* filename, CSocket* client, vector<inputFile> files)
{
	/*ifstream f;
	f.open(filename, ios::binary);
	int MsgSize = 512;
	if (!f.is_open())
	{
		cout << "Error openning file!" << endl;
	}
	char* temp_file = new char[512];
	f.seekg(0, ios::end);
	long long size_file = f.tellg();
	f.seekg(0, ios::beg);
	string file_size = to_string(size_file);
	int size_str = file_size.length();
	Connector->Send(&size_str, sizeof(size_str), 0);
	Connector->Send(file_size.c_str(), size_str, 0);
	long long byte_read = 0;
	while ((byte_read + 512) <= size_file)
	{
		f.read(temp_file, 512);
		Connector->Send(&MsgSize, sizeof(MsgSize), 0);
		Connector->Send(temp_file, MsgSize, 0);
		byte_read += 512;
	}
	int byte_remain = size_file - byte_read;
	if (byte_remain != 0)
	{
		f.read(temp_file, byte_remain);
		Connector->Send(&byte_remain, sizeof(byte_remain), 0);
		Connector->Send(temp_file, byte_remain, 0);
	}
	delete[] temp_file;
	f.close();
	MsgSize = 10;
	Connector->Send(&MsgSize, sizeof(MsgSize), 0);
	const char* stopFlag = "completed";
	Connector->Send(stopFlag, MsgSize, 0);*/
	vector<ifstream> input_files_stream;
	for (int i = 0; i < files.size(); i++) {
		input_files_stream[i].open(files[i].name, ios::binary);
	}
	int MsgSize;
	char* temp;
	int index = 0;
	bool* flag = new bool[files.size()];
	for (int i = 0; i < files.size(); i++) {
		flag[i] = false;
	}
	while(1) {
		if (flag[index] == false) {
			if (files[index].priority == "Critical") {
				for (int i = 0; i < 10; i++) {

					client.Receive((char*)&MsgSize, sizeof(MsgSize), 0);
					temp = new char[MsgSize];
					client.Receive(temp, MsgSize, 0);
					output_files_stream[index].write(temp, MsgSize);
					delete[] temp;
				}	
			}
			else if (files[index].priority == "High") {
				for (int i = 0; i < 4; i++) {
					client.Receive((char*)&MsgSize, sizeof(MsgSize), 0);
					temp = new char[MsgSize];
					client.Receive(temp, MsgSize, 0);
					output_files_stream[index].write(temp, MsgSize);
					delete[] temp;
				}				
			}
			else if (files[index].priority == "Normal") {
				client.Receive((char*)&MsgSize, sizeof(MsgSize), 0);
				temp = new char[MsgSize];
				client.Receive(temp, MsgSize, 0);
				output_files_stream[index].write(temp, MsgSize);
				delete[] temp;
			}
			//if tellg == xu li doi flag
			/*int temp1 = tellg();
			fseekg(ios::end);
			int temp2 = tellg*/

		}
		index++;
		if (index == files.size()) index = 0;

		bool check_all = true;
		for (int i = 0; i < files.size(); i++) {
			if (flag[i] == false) {
				index = i;
				check_all = false;
				break;
			}
		}

		if (check_all == true) {
			break;
		}
	}


	for (int i = 0; i < files.size(); i++) {
		input_files_stream[i].close;
	}
}