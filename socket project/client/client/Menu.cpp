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

void send_files_need_download_to_server(CSocket& client, vector<File> files) {
	int MsgSize;
	for (int i = 0; i < files.size(); i++) {
		MsgSize = files[i].filename.size();
		client.Send(&MsgSize, sizeof(MsgSize), 0);
		client.Send(files[i].filename.c_str(), files[i].filename.size(), 0);
		string pos = to_string(files[i].position);
		MsgSize = pos.size();
		client.Send(&MsgSize, sizeof(MsgSize), 0);
		client.Send(pos.c_str(), MsgSize, 0);
		MsgSize = files[i].priority.size();
		client.Send(&MsgSize, sizeof(MsgSize), 0);
		client.Send(files[i].priority.c_str(), files[i].priority.size(), 0);
	}
	const char * completed = "xong";
	MsgSize = strlen(completed);
	client.Send(&MsgSize, sizeof(MsgSize), 0);
	client.Send(completed, MsgSize, 0);
}

vector<long long> receiveFilesize(CSocket& client)
{
	vector<long long> file_size;
	int MsgSize;
	char* temp;
	while (1)
	{
		client.Receive((char*)&MsgSize, sizeof(int), 0);
		temp = new char[MsgSize + 1];
		client.Receive(temp, MsgSize, 0);
		temp[MsgSize] = '\0';
		if (strcmp(temp, "completed") != 0)
		{
			file_size.push_back(stoll((string)temp));
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

void receiveFile(vector<File>& files, CSocket& client) {
	vector<long long> file_size = receiveFilesize(client);
	for (int i = 0; i < files.size(); i++) {
		setCursorPosition(files[i].download_cursor.X, files[i].download_cursor.Y);
		cout << "Downloading " << files[i].filename << " ...";
	}
	int MsgSize;
	char* temp;

	int size_flag;
	char* flag_msg;
	int index = 0;
	ofstream fout;
	while (index < files.size()) {
		string location = "output/";
		location += files[index].filename;
		if (files[index].new_file == true)
		{
			fout.open(location.c_str(), ios::binary);
			files[index].new_file = false;
		}
		else
		{
			fout.open(location.c_str(), ios::binary | ios::app);
		}
		if (files[index].priority == "CRITICAL") {
			for (int i = 0; i < 10; i++) {
				client.Receive((char*)&MsgSize, sizeof(MsgSize), 0);
				temp = new char[MsgSize];
				int received;
				int cnt = 0;
				while (cnt < MsgSize)
				{
					received = client.Receive(temp, MsgSize, 0);
					fout.write(temp, received);
					cnt += received;
				}

				const char* nhan_het = "nhan het";
				int received_size = strlen(nhan_het);
				client.Send(&received_size, sizeof(received_size), 0);
				client.Send(nhan_het, received_size, 0);

				client.Receive((char*)&size_flag, sizeof(size_flag), 0);
				flag_msg = new char[size_flag + 1];
				client.Receive(flag_msg, size_flag, 0);
				flag_msg[size_flag] = '\0';
				setCursorPosition(files[index].download_cursor.X + 25 + files[index].filename.size(), files[index].download_cursor.Y);
				long long curpos = fout.tellp();
				float download = (float)(curpos) / (float)(file_size[index]);
				cout << fixed << setprecision(0) << download * 100 << "%" << flush;
				this_thread::sleep_for(as);
				if (strcmp(flag_msg, "done") == 0)
				{
					files[index].send_all_bytes = true;
					delete[] flag_msg;
					delete[] temp;
					break;
				}
				delete[] flag_msg;
				delete[] temp;
			}
		}
		else if (files[index].priority == "HIGH") {

			for (int i = 0; i < 4; i++) {
				client.Receive((char*)&MsgSize, sizeof(MsgSize), 0);
				temp = new char[MsgSize];
				int received;
				int cnt = 0;
				while (cnt < MsgSize)
				{
					received = client.Receive(temp, MsgSize, 0);
					fout.write(temp, received);
					cnt += received;
				}

				const char* nhan_het = "nhan het";
				int received_size = strlen(nhan_het);
				client.Send(&received_size, sizeof(received_size), 0);
				client.Send(nhan_het, received_size, 0);

				client.Receive((char*)&size_flag, sizeof(size_flag), 0);
				flag_msg = new char[size_flag + 1];
				client.Receive(flag_msg, size_flag, 0);
				flag_msg[size_flag] = '\0';
				setCursorPosition(files[index].download_cursor.X + 25 + files[index].filename.size(), files[index].download_cursor.Y);
				long long curpos = fout.tellp();
				float download = (float)(curpos) / (float)(file_size[index]);
				cout << fixed << setprecision(0) << download * 100 << "%" << flush;
				this_thread::sleep_for(as);
				if (strcmp(flag_msg, "done") == 0)
				{
					files[index].send_all_bytes = true;
					delete[] flag_msg;
					delete[] temp;
					break;
				}
				delete[] flag_msg;
				delete[] temp;
			}
		}
		else if (files[index].priority == "NORMAL") {
			client.Receive((char*)&MsgSize, sizeof(MsgSize), 0);
			temp = new char[MsgSize];
			int received;
			int cnt = 0;
			while (cnt < MsgSize)
			{
				received = client.Receive(temp, MsgSize, 0);
				fout.write(temp, received);
				cnt += received;
			}

			const char* nhan_het = "nhan het";
			int received_size = strlen(nhan_het);
			client.Send(&received_size, sizeof(received_size), 0);
			client.Send(nhan_het, received_size, 0);

			client.Receive((char*)&size_flag, sizeof(size_flag), 0);
			flag_msg = new char[size_flag + 1];
			client.Receive(flag_msg, size_flag, 0);
			flag_msg[size_flag] = '\0';
			if (strcmp(flag_msg, "done") == 0)
			{
				files[index].send_all_bytes = true;
			}
			setCursorPosition(files[index].download_cursor.X + 25 + files[index].filename.size(), files[index].download_cursor.Y);
			long long curpos = fout.tellp();
			float download = (float)(curpos)/ (float)(file_size[index]);
			cout << fixed << setprecision(0) << download * 100 << "%" << flush;
			this_thread::sleep_for(as);
			delete[] flag_msg;
			delete[] temp;
		}
		files[index].position = fout.tellp();
		index++;
		fout.close();
	}
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

void send_start(CSocket& client) {
	const char* start = "start";
	int size = strlen(start);
	client.Send(&size, sizeof(int), 0);
	client.Send(start, size, 0);
}
