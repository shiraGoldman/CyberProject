// Hooked.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Windows.h"

int main()
{
	while (true)
	{
		Sleep(5000);
		MessageBoxA(NULL, "original call", "hello", MB_OK);
	}
    return 0;
}

