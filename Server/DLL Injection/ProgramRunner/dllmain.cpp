// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "ProgramRunner.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	DllInjector dll;

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		//dll.runProgram();
	case DLL_THREAD_ATTACH:
		MessageBox(NULL, L"hook called", L"hehe", MB_OK);
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

