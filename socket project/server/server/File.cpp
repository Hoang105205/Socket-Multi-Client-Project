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

bool check_finish(ifstream& f)
{
    int temp1 = f.tellg();
    f.seekg(ios::end);
    int temp2 = f.tellg();
    if (temp1 == temp2)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void sendFilesize(CSocket* client, vector<inputFile> files)
{
    ifstream f;
    int size;
    string temp;
    int MsgSize;
    for (int i = 0; i < files.size(); i++)
    {
        f.open(files[i].name, ios::binary);
        f.seekg(0, ios::end);
        size = f.tellg();
        temp = to_string(size);
        MsgSize = temp.length();
        client->Send(&MsgSize, sizeof(int), 0);
        client->Send((char*)&temp, MsgSize, 0);
        f.close();
    }
    const char* stopflag = "completed";
    MsgSize = strlen(stopflag);
    client->Send(&MsgSize, sizeof(int), 0);
    client->Send(stopflag, MsgSize, 0);
}

void sendFile(CSocket* client, vector<inputFile> files)
{
    sendFilesize(client, files);
    ifstream* input_files_stream = new ifstream[files.size()];
    for (int i = 0; i < files.size(); i++) {
        input_files_stream[i].open(files[i].name, ios::binary);
    }
    int MsgSize = 1048576;
    char* temp;
    int index = 0;
    bool* flag = new bool[files.size()];
    for (int i = 0; i < files.size(); i++) {
        flag[i] = false;
    }
    while (1) {
        if (flag[index] == false) {
            if (files[index].priority == "CRITICAL") {
                for (int i = 0; i < 10; i++) {
                    temp = new char[MsgSize];
                    input_files_stream[index].read(temp, MsgSize);
                    client->Send(&MsgSize, sizeof(MsgSize), 0);
                    client->Send(temp, MsgSize, 0);
                    delete[] temp;
                    if (check_finish(input_files_stream[index]))
                    {
                        flag[index] = true;
                        const char* complete = "Completed";
                        int size = strlen(complete);
                        client->Send(&size, sizeof(size), 0);
                        client->Send(complete, size, 0);
                    }
                }
            }
            else if (files[index].priority == "HIGH") {
                for (int i = 0; i < 4; i++) {
                    temp = new char[MsgSize];
                    input_files_stream[index].read(temp, MsgSize);
                    client->Send(&MsgSize, sizeof(MsgSize), 0);
                    client->Send(temp, MsgSize, 0);
                    delete[] temp;
                    if (check_finish(input_files_stream[index]))
                    {
                        flag[index] = true;
                        const char* complete = "Completed";
                        int size = strlen(complete);
                        client->Send(&size, sizeof(size), 0);
                        client->Send(complete, size, 0);
                    }
                }
            }
            else if (files[index].priority == "NORMAL") {
                temp = new char[MsgSize];
                input_files_stream[index].read(temp, MsgSize);
                client->Send(&MsgSize, sizeof(MsgSize), 0);
                client->Send(temp, MsgSize, 0);
                delete[] temp;
                if (check_finish(input_files_stream[index]))
                {
                    flag[index] = true;
                    const char* complete = "Completed";
                    int size = strlen(complete);
                    client->Send(&size, sizeof(size), 0);
                    client->Send(complete, size, 0);
                }
            }
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
        input_files_stream[i].close();
    }
    delete[] input_files_stream;
	delete[] flag;
}