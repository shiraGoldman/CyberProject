#include "stdafx.h"
#include "DLL.h"

//int newFunc(const char * format, ...)
//{
//	MessageBoxA(NULL, "hook called", "hehe", MB_OK);
//	return 0;
//}

int WINAPI newFunc(HWND hwnd, LPCTSTR lptext, LPCTSTR lpCaption, UINT uType)
{
	MessageBoxA(NULL, "hook called", "hehe", MB_OK);
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
		//IAThooking(GetModuleHandleA(NULL), TARGET_FUNCTION, (void *)sourceAddr);
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}
