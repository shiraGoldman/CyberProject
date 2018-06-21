#include "stdafx.h"
#include "DLL.h"

int newFunc(FILE* stream, const char * format, ...)
{
	MessageBoxA(NULL, "hook called", "hehe", MB_OK);
	return 0;
}
BOOL APIENTRY DllMain(HINSTANCE hInst, DWORD reason, LPVOID reserved)
{
	//IAT::IATDll dll;
	LPCTSTR message = L"This is a message";
	LPCTSTR title = L"This is a message";
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
		MessageBox(NULL, message, title, MB_OK);
		//dll.IAThooking(GetModuleHandleA(NULL), TARGET_FUNCTION, newFunc);
		break;
	case DLL_PROCESS_DETACH:
		MessageBox(NULL, L"dll main", L"hehe", MB_OK);
		//IAThooking(GetModuleHandleA(NULL), TARGET_FUNCTION, (void *)sourceAddr);
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}
