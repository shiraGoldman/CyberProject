#include "WindowsSocket.h"

using namespace std;
void WindowsSocket::startup()
{
	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0) {
		throw new exception("WSAStartup failed with error: %d\n", result);
	}
}

void WindowsSocket::cleanup()
{
	WSACleanup();
}