#include "stdafx.h"
#include "DLL.h"

int WINAPI newFunc(HWND hwnd, LPCSTR lptext, LPCSTR lpCaption, UINT uType)
{
	MessageBoxA(NULL, "hook called", "hehe", MB_OK);
	MessageBoxA(hwnd, lptext, lpCaption, uType);
	return 0;
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
