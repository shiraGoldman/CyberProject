#pragma once
/**
C++ client example using sockets
*/

#include<iostream>    //cout
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

using namespace std;

class SocketManager
{
private:
	addrinfo* addrInfo;

public:
	SocketManager();
	void setAddrInfo(addrinfo*);
	void freeAddrInfo();
	SOCKET create();
	void close(SOCKET);
	int conn(SOCKET);
	int send_data(SOCKET, const char*);
	int receive(SOCKET, char*, int);
};

