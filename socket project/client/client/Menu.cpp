#include "Menu.h"
atomic<bool> offFlag(false);
mutex mtx;;
queue<vector<inputFile>> file_download;
namespace fs = filesystem;
using attoseconds = std::chrono::duration<long long, std::atto>;
attoseconds as(1000);
auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(as);

void signal_callback_handler(int signum) {
	exit(signum);
}

vector<info> ReceiveFiles_canbedownloaded(CSocket& ClientSocket) {

	int MsgSize;
	char* temp;
	int i = 0;

	cout << "Server: " << endl;
	vector<info> files;
	info temp_file;

	int receivedNumber;
	ClientSocket.Receive((char*)&receivedNumber, sizeof(receivedNumber));
	for (int i = 1; i <= receivedNumber * 2; i++)
	{
		// Nhan thong diep tu Server
		ClientSocket.Receive((char*)&MsgSize, sizeof(int), 0); // Neu nhan loi thi tra ve la SOCKET_ERROR.			
		temp = new char[MsgSize + 1];
		ClientSocket.Receive((char*)temp, MsgSize, 0);
		temp[MsgSize] = '\0';
		if (i % 2 != 0) {
			temp_file.name = string(temp);
			cout << temp << " ";
		}
		else {
			temp_file.size = string(temp);
			cout << temp << endl;
			files.push_back(temp_file);
		}
		delete[]temp;
	}
	return files;
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
				string temp;
				getline(file, temp, '\n');
				if (temp == "") break;

				stringstream sstream(temp);
				size_t found = temp.find(' ');
				if (found != string::npos) {
					getline(sstream, line.name, ' ');
					getline(sstream, line.priority);
				}
				else
				{
					getline(sstream, line.name, ' ');
					line.priority = "";
				}
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
	const char * completed = "xong";
	MsgSize = strlen(completed);
	client.Send(&MsgSize, sizeof(MsgSize), 0);
	client.Send(completed, MsgSize, 0);
}

vector<int> receiveFilesize(CSocket& client)
{
	vector<int> file_size;
	int MsgSize;
	char* temp;
	while (1)
	{
		client.Receive((char*)&MsgSize, sizeof(int), 0);
		temp = new char[MsgSize];
		client.Receive(temp, MsgSize, 0);
		if (strcmp(temp, "completed") != 0)
		{
			file_size.push_back(atoi(temp));
			delete[] temp;
		}
		else
		{
			delete[] temp;
			break;
		}
	}
	return file_size;
}

void receiveFile(vector<inputFile> files, CSocket& client, COORD current) {
	vector<int> file_size = receiveFilesize(client);
	vector<float> percent;
	for (int i = 0; i < file_size.size(); i++)
	{
		float temp = static_cast<float>(1048576) / file_size[i];
		percent.push_back(temp);
	}
	vector<ofstream> output_files_stream;
	vector<COORD> temp_cursor;
	for (int i = 0; i < files.size(); i++) {
		setCursorPosition(current.X, current.Y + i);
		cout << "Downloading " << files[i].name << " ...";
		temp_cursor.push_back(getCursorPosition());
		cout << endl;
		output_files_stream[i].open(files[i].name, ios::binary);
	}
	int* download = new int[files.size()];
	for (int i = 0; i < files.size(); i++)
	{
		download[i] = 0;
	}
	int MsgSize;
	char* temp;
	int index = 0;
	bool* flag = new bool[files.size()];
	for (int i = 0; i < files.size(); i++)
	{
		flag[i] = false;
	}
	while (1) {
		if (files[index].priority == "CRITICAL") {
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
				SetCursorPos(temp_cursor[index].X + 5,temp_cursor[index].Y);
				download[index] += percent[index];
				cout << fixed << setprecision(0) << download[index] * 100 << "%" << flush;
				this_thread::sleep_for(as);
				delete[] temp;
			}
		}
		else if (files[index].priority == "HIGH") {
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
				SetCursorPos(temp_cursor[index].X + 5, temp_cursor[index].Y);
				download[index] += percent[index];
				cout << fixed << setprecision(0) << download[index] * 100 << "%" << flush;
				this_thread::sleep_for(as);
				delete[] temp;
			}
		}
		else if (files[index].priority == "NORMAL") {
			client.Receive((char*)&MsgSize, sizeof(MsgSize), 0);
			temp = new char[MsgSize];
			client.Receive(temp, MsgSize, 0);
			if (temp == "completed")
			{
				flag[index] = true;
				break;
			}
			output_files_stream[index].write(temp, MsgSize);
			SetCursorPos(temp_cursor[index].X + 5, temp_cursor[index].Y);
			download[index] += percent[index];
			cout << fixed << setprecision(0) << download[index] * 100 << "%" << flush;
			this_thread::sleep_for(as);
			delete[] temp;
		}
		index++;
		if (index == files.size()) index = 0;
		bool checkall = true;
		for (int j = 0; j < files.size(); j++)
		{
			if (flag[j] == false)
			{
				index = j;
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
	delete[] flag;
	delete[] download;
}

void set_up() {
	if (fs::create_directory("output")) {
		cout << "Folder created successfully.\n";
	}
	else {
		cout << "Folder already exists or could not be created.\n";
	}
	ifstream input;
	string input_str = "input.txt";
	input.open(input_str.c_str(), ios::app);
	input.close();
}
