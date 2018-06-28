#pragma once
#include <Windows.h>
#include <string>
#include <tchar.h>
#include <iostream>
#include "stdafx.h"


#ifdef DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif


using namespace std;


class DllInjector
{
public:
	void DLL_API runProgram();
	
};

