#include "Socket.h"

using namespace std;

Socket::Socket()
{

}

void Socket::setAddrInfo(addrinfo* addrInfo)
{
	this->addrInfo = addrInfo;
}

void Socket::freeAddrInfo()
{
	freeaddrinfo(addrInfo);
}
SOCKET Socket::create()
{
	SOCKET connectSocket = socket(addrInfo->ai_family, addrInfo->ai_socktype,
		addrInfo->ai_protocol);
	if (connectSocket == INVALID_SOCKET) {
		throw new exception("socket failed with error: %ld\n", WSAGetLastError());
		
	}
	
	return connectSocket;
}

void Socket::close(SOCKET socket)
{
	closesocket(socket);
}
/**
Connect to a host on a certain port number
*/
bool Socket::conn(SOCKET socket)
{
	return connect(socket, addrInfo->ai_addr, (int)addrInfo->ai_addrlen);
}

/**
Send data to the connected host
*/
int Socket::send_data(SOCKET socket, char* message)
{
	int result = send(socket, message, (int)strlen(message), 0);
	if (result == SOCKET_ERROR) {
		closesocket(socket);
		throw new exception("send failed with error: %d\n", GetLastError());
		// WSACleanup()
	}

	return result;
}

/**
Receive data from the connected host
*/
int Socket::receive(SOCKET socket, char* buffer /*out*/, int bufferlen)
{
	int result = recv(socket, buffer, bufferlen, 0);

	if (result == 0)
		throw new exception("Connection closed\n");
	else if (result < 0 )
		throw new exception("recv failed with error: %d\n", WSAGetLastError());

	return result;
}