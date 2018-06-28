#include "stdafx.h"

#define DLL_EXPORTS

void DllInjector::runProgram()
{
	/*const char * cmd = "client.exe";

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
	_pclose(pipe);*/

	LPTSTR szCmdline = _tcsdup(TEXT("C:\\Users\\admin\\Desktop\\Client.exe"));


	STARTUPINFO info = { sizeof(info) };
	PROCESS_INFORMATION processInfo;
	HANDLE hprocess = NULL;
	if (CreateProcess(NULL, szCmdline, NULL, NULL, TRUE, 0, NULL, NULL, (LPSTARTUPINFO)&info, &processInfo))
	{
		hprocess = processInfo.hProcess;
	}
	if (!hprocess)
	{
		cout << GetLastError();
	}
}
