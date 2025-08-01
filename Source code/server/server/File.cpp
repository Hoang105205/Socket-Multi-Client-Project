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

long long get_file_size(ifstream &ifstream_filename) {
	ifstream_filename.seekg(0, ios::end);
	long long a = ifstream_filename.tellg();
	return a;
}
