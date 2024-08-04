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

void HandleReceiveError(CSocket& socket) {
	int error = socket.GetLastError();
	switch (error) {
	case WSAEWOULDBLOCK:
		std::cerr << "The socket is marked as non-blocking and the operation would block.\n";
		break;
	case WSAECONNRESET:
		std::cerr << "Connection reset by peer.\n";
		break;
	case WSAENOTCONN:
		std::cerr << "The socket is not connected.\n";
		break;
	case WSAESHUTDOWN:
		std::cerr << "The socket has been shut down.\n";
		break;
		// Add more cases as needed
	default:
		std::cerr << "Receive failed with error: " << error << "\n";
		break;
	}
}

void receiveFile(vector<File>& files, CSocket& client, COORD current) {
	vector<long long> file_size = receiveFilesize(client);


	vector<float> percent;
	//for (int i = 0; i < file_size.size(); i++)
	//{
	//	float temp = static_cast<float>(1048576) / file_size[i];
	//	percent.push_back(temp);
	//}
	//vector<COORD> temp_cursor;
	//for (int i = 0; i < files.size(); i++) {
	//	setCursorPosition(current.X, current.Y + i);
	//	cout << "Downloading " << files[i].filename << " ...";
	//	temp_cursor.push_back(getCursorPosition());
	//	cout << endl;
	//}
	//int* download = new int[files.size()];
	//for (int i = 0; i < files.size(); i++)
	//{
	//	download[i] = 0;
	//}
	int MsgSize;
	char* temp;

	int size_flag;
	char* flag_msg;
	int index = 0;
	ofstream fout;
	while (index < files.size()) {
		if (files[index].new_file == true)
		{
			fout.open(files[index].filename.c_str(), ios::binary);
			files[index].new_file = false;
		}
		else
		{
			fout.open(files[index].filename.c_str(), ios::binary | ios::app);
		}
		if (files[index].priority == "CRITICAL") {
			for (int i = 0; i < 10; i++) {
				client.Receive((char*)&MsgSize, sizeof(MsgSize), 0);
				temp = new char[MsgSize];
				client.Receive(temp, MsgSize, 0);
				fout.write(temp, MsgSize);

				/*SetCursorPos(temp_cursor[index].X + 5, temp_cursor[index].Y);
				download[index] += percent[index];
				cout << fixed << setprecision(0) << download[index] * 100 << "%" << flush;
				this_thread::sleep_for(as);*/

				client.Receive((char*)&size_flag, sizeof(size_flag), 0);
				flag_msg = new char[size_flag + 1];
				client.Receive(flag_msg, size_flag, 0);
				flag_msg[size_flag] = '\0';
				if (strcmp(flag_msg, "done") == 0)
				{
					files[index].send_all_bytes = true;
					break;
				}
				cout << flag_msg << endl;
				delete[] flag_msg;
				delete[] temp;
			}
		}
		else if (files[index].priority == "HIGH") {
			for (int i = 0; i < 4; i++) {
				client.Receive((char*)&MsgSize, sizeof(MsgSize), 0);
				temp = new char[MsgSize];
				client.Receive(temp, MsgSize, 0);
				fout.write(temp, MsgSize);

				/*SetCursorPos(temp_cursor[index].X + 5, temp_cursor[index].Y);
				download[index] += percent[index];
				cout << fixed << setprecision(0) << download[index] * 100 << "%" << flush;
				this_thread::sleep_for(as);*/

				client.Receive((char*)&size_flag, sizeof(size_flag), 0);
				flag_msg = new char[size_flag + 1];
				client.Receive(flag_msg, size_flag, 0);
				flag_msg[size_flag] = '\0';
				if (strcmp(flag_msg, "done") == 0)
				{
					files[index].send_all_bytes = true;
					break;
				}
				cout << flag_msg << endl;
				delete[] flag_msg;
				delete[] temp;
			}
		}
		else if (files[index].priority == "NORMAL") {
			if (client.Receive((char*)&MsgSize, sizeof(MsgSize), 0) ==  SOCKET_ERROR)
			{
				HandleReceiveError(client);
			}
			cout << endl << MsgSize << endl;

			temp = new char[MsgSize];
			if (client.Receive(temp, MsgSize, 0) == SOCKET_ERROR) {
				HandleReceiveError(client);
			}
			cout << endl << temp << endl;

			fout.write(temp, MsgSize);
			/*SetCursorPos(temp_cursor[index].X + 5, temp_cursor[index].Y);
			download[index] += percent[index];
			cout << fixed << setprecision(0) << download[index] * 100 << "%" << flush;
			this_thread::sleep_for(as);*/

			if (client.Receive((char*)&size_flag, sizeof(size_flag), 0) == SOCKET_ERROR)
			{
				HandleReceiveError(client);
			}
			cout << endl << size_flag << endl;

			flag_msg = new char[size_flag + 1];
			if (client.Receive(flag_msg, size_flag, 0) == SOCKET_ERROR)
			{
				HandleReceiveError(client);
			}
			flag_msg[size_flag] = '\0';
			if (strcmp(flag_msg, "done") == 0)
			{
				files[index].send_all_bytes = true;
			}
			cout << endl << flag_msg << endl;
			delete[] flag_msg;
			delete[] temp;
		}
		files[index].position = fout.tellp(); 
		index++;
		fout.close();
	}
	/*delete[] download;*/
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
