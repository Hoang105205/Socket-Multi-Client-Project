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

void writeInfo(char infos[])
{
	ofstream f;
	f.open("input.txt", ios::app);
	if (!f.is_open())
	{
		cout << "Khong mo duoc file";
		return;
	}
	if (isFileEmpty("input.txt") == false)
	{
		f << endl;
	}
	for (int i = 0; i < strlen(infos); i++)
	{
		if (infos[i] == ',')
		{
			f << endl;
			i++;
		}
		f << infos[i];
	}
	f.close();
	return;
}

vector<string> InitListIfExisted(string filename)
{
	vector<string> List_temp;
	ifstream f(filename.c_str());
	if (!f.is_open())
	{
		return List_temp;
	}
	string line = "";
	while (!f.eof())
	{
		getline(f, line, '\n');
		if (line == "") {
			break;
		}
		List_temp.push_back(line);
	}
	f.close();
	return List_temp;
}



void writeFile(vector<string> file, string filename, COORD cursorPos)
{
	ofstream f;
	f.open(filename.c_str(), ios::binary);
	float percent = 1 / file.size();
	float download = 0;
	if (!f.is_open())
	{
		cout << "Error!" << endl;
		return;
	}
	int cnt = 0;
	for (int i = 0; i < file.size(); i++)
	{
		cnt++;
		cout << cnt << endl;
		f.write(file[i].c_str(), 512);
		/*download += percent;*/
		/*setCursorPosition(cursorPos.X + 5, cursorPos.Y);
		cout << download << "%" << flush;
		this_thread::sleep_for(std::chrono::seconds(1));*/
	}
	f.close();
}