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

bool checkInfo(string temp, vector<info> infos)
{
	bool flag = false;
	for (int i = 0; i < infos.size(); i++)
	{
		if (temp == infos[i].name)
		{
			flag = true;
		}
	}
	return flag;
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

void ReceivedFileDownload(CSocket& ClientSocket, string filename, COORD cursorPos) {
	int MsgSize;
	long long file_size;
	char* temp;
	ClientSocket.Receive((char*)&MsgSize, sizeof(MsgSize), 0);
	temp = new char[MsgSize];
	ClientSocket.Receive(temp, MsgSize, 0);
	file_size = stoll((string)temp);
	ofstream f;
	string location = "output/";
	location = location + filename;
	f.open(location.c_str(), ios::binary);
	if (!f.is_open()) {
		cout << "Error opening file!" << endl;
		return;
	}
	float percent = static_cast<float>(512) / file_size;
	float download = 0;
	delete[] temp;
	setCursorPosition(cursorPos.X, cursorPos.Y);
	cout << "Downloading " << filename << " ...";
	do
	{
		ClientSocket.Receive((char*)&MsgSize, sizeof(int), 0);
		temp = new char[MsgSize];
		ClientSocket.Receive(temp, MsgSize, 0);
		if (strcmp(temp, "completed") != 0) {
			f.write(temp, MsgSize);
			setCursorPosition(cursorPos.X + filename.length() + 20, cursorPos.Y);
			download += percent;
			cout << fixed << setprecision(0) << download * 100 << "%" << flush;
			this_thread::sleep_for(as);
			delete[] temp;
		}
		else {
			delete[] temp;
			break;
		}
	} while (1);
	f.close();
}

vector<string> readNewFileAdded(string filename, vector<string>& fileList, vector<info> List)
{
	ifstream file(filename.c_str());
	vector<string> file_download;
	if (!file.is_open()) {
		return file_download;
	}
	string line = "";
	vector<string> List_temp;
	while (!file.eof())
	{
		getline(file, line, '\n');
		if (line == "") {
			break;
		}
		List_temp.push_back(line);
	}
	file.close();

	if (List_temp.size() != fileList.size())
	{
		for (int i = fileList.size(); i < List_temp.size(); i++)
		{
			fileList.push_back(List_temp[i]);
			if (checkInfo(List_temp[i], List)) file_download.push_back(List_temp[i]);
		}
	}
	return file_download;
}

void resumeThread() {
	unique_lock<mutex> lk(mtx);
	pauseFlag = false;
	cv.notify_one();
}

void send_files_need_download_to_server(CSocket& client, vector<string> filename) {
	int MsgSize;
	for (int i = 0; i < filename.size(); i++) {
		MsgSize = filename[i].length();
		client.Send(&MsgSize, sizeof(MsgSize), 0);
		client.Send(filename[i].c_str(), MsgSize, 0);
	}
	const char* complete_str = "xong";
	MsgSize = strlen(complete_str);
	client.Send(&MsgSize, sizeof(MsgSize), 0);
	client.Send(complete_str, MsgSize, 0);
}

void receiveFile(vector<inputFile> files, CSocket& client, COORD current) {
	vector<string> output_files;
	vector<ofstream> output_files_stream;
	string file = "file";
	for (int i = 0; i < files.size(); i++) {
		setCursorPosition(current.X, current.Y + i);
		cout << "Downloading " << files[i].name << " ...";
		output_files[i] = "file" + to_string(i + 1);
		output_files_stream[i].open(output_files[i], ios::binary);
	}
	int MsgSize;
	char* temp;
	for (int i = 0; i < files.size(); i++) {
		if (files[i].priority == "Critical") {
			for (int i = 0; i < 3; i++) {
				client.Receive((char*)&MsgSize, sizeof(MsgSize), 0);
				temp = new char[MsgSize];
				client.Receive(temp, MsgSize, 0);
				output_files_stream[i].write(temp, MsgSize);
			}
		}
		else if (files[i].priority == "High") {
			for (int i = 0; i < 2; i++) {
				client.Receive((char*)&MsgSize, sizeof(MsgSize), 0);
				temp = new char[MsgSize];
				client.Receive(temp, MsgSize, 0);
				output_files_stream[i].write(temp, MsgSize);
			}
		}
		else if (files[i].priority == "Normal") {
			client.Receive((char*)&MsgSize, sizeof(MsgSize), 0);
			temp = new char[MsgSize];
			client.Receive(temp, MsgSize, 0);
			output_files_stream[i].write(temp, MsgSize);
		}
	}
	for (int i = 0; i < files.size(); i++) {
		output_files_stream[i].close();
	}
}