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
        f.open(files[i].name.c_str(), ios::binary);
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
