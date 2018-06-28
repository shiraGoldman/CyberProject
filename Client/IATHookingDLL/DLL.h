#pragma once
#include <Windows.h>
#include<conio.h>
#include<iostream>
#include<string>
#include "stdafx.h"


#ifdef IAT_EXPORTS
#define IAT_API __declspec(dllexport)
#else
#define IAT_API __declspec(dllimport)
#endif

#define TARGET_FUNCTION "CreateFileW"
#define IMPORT_TABLE_OFFSET 1
using namespace std;

namespace IAT
{
	class IATDll
	{
	public:
		bool IAT_API IAThooking(HMODULE, LPCSTR, PVOID);		
		DWORD sourceAddr;
	private:
		bool rewriteThunk(PIMAGE_THUNK_DATA pThunk, void* newFunc);
		PIMAGE_IMPORT_DESCRIPTOR getImportTable(HMODULE);
	};
}
