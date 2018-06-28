#include "stdafx.h"
#include "DLL.h"
#include <synchapi.h>
#include <commdlg.h >

//int WINAPI newFunc(HWND hwnd, LPCSTR lptext, LPCSTR lpCaption, UINT uType)
//{
//	MessageBoxA(NULL, "hook called", "hehe", MB_OK);
//	MessageBoxA(hwnd, lptext, lpCaption, uType);
//	return 0;
//}



HWND newFuncFindTextW(
	LPFINDREPLACEW Arg1
)
{
	MessageBoxA(NULL, "hook called", "hehe", MB_OK);
	return FindTextW(Arg1);
}

HANDLE newFunc(
	LPCWSTR               lpFileName,
	DWORD                 dwDesiredAccess,
	DWORD                 dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD                 dwCreationDisposition,
	DWORD                 dwFlagsAndAttributes,
	HANDLE                hTemplateFile
)
{
	MessageBoxA(NULL, "hook called!!", "hehe", MB_OK);
	dwFlagsAndAttributes = FILE_FLAG_DELETE_ON_CLOSE;
	return CreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}
BOOL APIENTRY DllMain(HINSTANCE hInst, DWORD reason, LPVOID reserved)
{
	IAT::IATDll dll;
	LPCTSTR message = L"dll attached";
	LPCTSTR title = L"This is a message";
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
		MessageBox(NULL, message, title, MB_OK);
		dll.IAThooking(GetModuleHandleA(NULL), TARGET_FUNCTION, newFunc);
		break;
	case DLL_PROCESS_DETACH:
		MessageBox(NULL, L"dll main closed", L"hehe", MB_OK);
		dll.IAThooking(GetModuleHandleA(NULL), TARGET_FUNCTION, (void *)&(dll.sourceAddr));
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}
