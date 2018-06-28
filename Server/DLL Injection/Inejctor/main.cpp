#include <windows.h>
#include <iostream>
#include <tlhelp32.h>

using namespace std;

DWORD GetProcId(char* ProcName);

DWORD ProcId = 0; // THIS IS OUR GLOBAL VARIABLE FOR THE PROC ID;

DWORD GetProcId(char* ProcName)
{
	PROCESSENTRY32   pe32;
	HANDLE         hSnapshot = NULL;

	pe32.dwSize = sizeof(PROCESSENTRY32);
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (Process32First(hSnapshot, &pe32))
	{
		do {
			if (strcmp((const char *)(pe32.szExeFile), ProcName) == 0)
				break;
		} while (Process32Next(hSnapshot, &pe32));
	}

	if (hSnapshot != INVALID_HANDLE_VALUE)
		CloseHandle(hSnapshot);

	ProcId = pe32.th32ProcessID;

	return ProcId;
}


int main()
{
	char * dllPath = "C:\\Users\\admin\\Documents\\Project\\CyberProject\\Server\\DLL Injection\\ProgramRunner.dll";

	DWORD processID = 10372; //GetProcId("CFF Explorer.exe");
	HANDLE hprocess = NULL;
	hprocess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
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

	HANDLE hthread = CreateRemoteThread(hprocess, NULL, NULL, (LPTHREAD_START_ROUTINE)
		LoadLibraryAtRemoteProcess, (LPVOID)dllPathRemoteAddress, NULL, NULL);

	CloseHandle(hprocess);
	CloseHandle(hthread);
	return 0;
}