#include "File.h"

using namespace std;

void setCursorPosition(int x, int y) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD position = { static_cast<SHORT>(x), static_cast<SHORT>(y) };
	SetConsoleCursorPosition(hConsole, position);
}

COORD getCursorPosition() {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
	GetConsoleScreenBufferInfo(hConsole, &bufferInfo);
	return bufferInfo.dwCursorPosition;
}

bool isFileEmpty(string filename) {
	ifstream file(filename, ios::binary | ios::ate);

	if (!file.is_open()) {
		cout << "Error!" << endl;
		return false;
	}

	int fileSize = file.tellg();
	file.close();

	return (fileSize == 0) ? true : false;
}

vector<inputFile> InitListIfExisted(string filename)
{
	vector<inputFile> List_temp;
	ifstream f(filename.c_str());
	if (!f.is_open())
	{
		return List_temp;
	}
	inputFile line;
	while (!f.eof())
	{
		string temp;
		getline(f, temp, '\n');
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
	f.close();
	return List_temp;
}

vector<File> clean_list(vector<File> files) {
	//kick nhung file da tai xong
	vector<File> res;
	int index = 0;
	while (index < files.size()) {
		if (files[index].send_all_bytes != true) {
			res.push_back(files[index]);
		}
		index++;
	}
	return res;
}

void merge_list(vector<File>& files, vector<inputFile> input, COORD& start) {
	for (int i = 0; i < input.size(); i++) {
		bool flag = true;
		for (int j = 0; j < files.size(); j++) {
			if (input[i].name == files[j].filename) {
				flag = false;
				if (input[i].priority < files[j].priority) {
					files[j].priority = input[i].priority;
				}
				break;
			}
		}
		if (flag == true) {
			File put;
			put.filename = input[i].name;
			put.priority = input[i].priority;
			put.new_file = true;
			put.send_all_bytes = false;
			put.position = 0;
			put.download_cursor = start;
			files.push_back(put);
			start.Y++;
		}
	}
}