#include "Menu.h"
atomic<bool> offFlag(false);
atomic<bool> pauseFlag(false);
condition_variable cv;
mutex mtx;
atomic<bool> DownFlag(true);
condition_variable cv_Download;
mutex mtx_Download;

using attoseconds = std::chrono::duration<long long, std::atto>;
attoseconds as(1000);
auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(as);

void signal_callback_handler(int signum) {
	exit(signum);
}


vector<string> StringToVector(string temp)
{
	vector<string> a;
	int cnt = 0;
	int mark = 0;
	for (int i = 0; i < temp.length(); i++)
	{
		if (temp[i] == ',')
		{
			a.push_back(temp.substr(mark, cnt));
			mark = i + 1;
			cnt = -1;
		}
		cnt++;
	}
	if (a.size() == 0)
	{
		a.push_back(temp);
	}
	else
	{
		a.push_back(temp.substr(mark, temp.length() - mark));
	}
	return a;
}


//Quang
bool checkInfo(inputFile temp, vector<info> infos, string level[])
{
	bool flag = false;
	for (int i = 0; i < infos.size(); i++)
	{
		if (temp.name == infos[i].name)
		{
			for (int i = 0; i < 3; i++)
			{
				if (temp.priority == level[i]) flag = true;
			}
		}
	}
	return flag;
}
void readNewFileAdded(string filename, vector<inputFile>& fileList, vector<info> List, string Level[])
{
	while (offFlag != true) {
		ifstream file(filename.c_str());
		if (file.is_open()) {
			inputFile line;
			vector<inputFile> List_temp;
			while (!file.eof())
			{
				getline(file, line.name, ' ');
				if (line.name == "") {
					break;
				}
				getline(file, line.priority, '\n');
				List_temp.push_back(line);
			}
			file.close();


			if (List_temp.size() != fileList.size())
			{
				unique_lock<mutex> lk(mtx);
				vector<inputFile> tmp;
				for (int i = fileList.size(); i < List_temp.size(); i++)
				{
					fileList.push_back(List_temp[i]);
					if (checkInfo(List_temp[i], List, Level))
					{
						tmp.push_back(List_temp[i]);
					}
				}
				file_download.push(tmp);
			}
		}
		// Sleep for 2 seconds
		this_thread::sleep_for(std::chrono::seconds(2));
	}
}
////////

void send_files_need_download_to_server(CSocket& client, vector<inputFile> files) {
	int MsgSize;
	for (int i = 0; i < files.size(); i++) {
		MsgSize = files[i].name.size();
		client.Send(&MsgSize, sizeof(MsgSize), 0);
		client.Send(files[i].name.c_str(), files[i].name.size(), 0);
		MsgSize = files[i].priority.size();
		client.Send(&MsgSize, sizeof(MsgSize), 0);
		client.Send(files[i].priority.c_str(), files[i].priority.size(), 0);
	}
	const char* completed = "xong";
	MsgSize = strlen(completed);
	client.Send(&MsgSize, sizeof(MsgSize), 0);
	client.Send(completed, MsgSize, 0);
}

void receiveFile(vector<inputFile> files, CSocket& client, COORD current) {
	vector<ofstream> output_files_stream;
	string file = "file";
	COORD temp_cursor = getCursorPosition();
	for (int i = 0; i < files.size(); i++) {
		setCursorPosition(current.X, current.Y + i);
		cout << "Downloading " << files[i].name << " ...";
		output_files_stream[i].open(files[i].name, ios::binary);
	}
	send_files_need_download_to_server(client, files);
	int MsgSize;
	char* temp;
	int index = 0;
	bool* flag = new bool[files.size()];
	for (int i = 0; i < file.size(); i++)
	{
		flag[i] = false;
	}
	while (1) {
		if (files[index].priority == "Critical") {
			for (int i = 0; i < 10; i++) {
				client.Receive((char*)&MsgSize, sizeof(MsgSize), 0);
				temp = new char[MsgSize];
				client.Receive(temp, MsgSize, 0);
				if (temp == "completed")
				{
					flag[index] = true;
					break;
				}
				output_files_stream[i].write(temp, MsgSize);
				delete[] temp;
			}
		}
		else if (files[index].priority == "High") {
			for (int i = 0; i < 4; i++) {
				client.Receive((char*)&MsgSize, sizeof(MsgSize), 0);
				temp = new char[MsgSize];
				client.Receive(temp, MsgSize, 0);
				if (temp == "completed")
				{
					flag[index] = true;
					break;
				}
				output_files_stream[i].write(temp, MsgSize);
				delete[] temp;
			}
		}
		else if (files[index].priority == "Normal") {
			client.Receive((char*)&MsgSize, sizeof(MsgSize), 0);
			temp = new char[MsgSize];
			client.Receive(temp, MsgSize, 0);
			if (temp == "completed")
			{
				flag[index] = true;
				break;
			}
			output_files_stream[index].write(temp, MsgSize);
			delete[] temp;
		}
		bool checkall = true;
		for (int j = 0; j < files.size(); j++)
		{
			if (flag[j] == false)
			{
				checkall = false;
				break;
			}
		}
		if (checkall == true)
		{
			break;
		}
	}
	for (int i = 0; i < files.size(); i++) {
		output_files_stream[i].close();
	}
}