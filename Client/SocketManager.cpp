#include "SocketManager.h"

using namespace std;

#define DEFAULT_BUFLEN 1050
#define OK_PACKET_SIZE 2

SocketManager::SocketManager()
{

}

void SocketManager::setAddrInfo(addrinfo* addrInfo)
{
	this->addrInfo = addrInfo;
}

void SocketManager::freeAddrInfo()
{
	freeaddrinfo(addrInfo);
}
SOCKET SocketManager::create()
{
	SOCKET connectSocket = socket(addrInfo->ai_family, addrInfo->ai_socktype,
		addrInfo->ai_protocol);
	if (connectSocket == INVALID_SOCKET) {
		throw new exception("socket failed with error: %ld\n", WSAGetLastError());
		
	}
	
	return connectSocket;
}

void SocketManager::close(SOCKET socket)
{
	closesocket(socket);
}
/**
Connect to a host on a certain port number
*/
int SocketManager::conn(SOCKET socket)
{
	return connect(socket, addrInfo->ai_addr, (int)addrInfo->ai_addrlen);
}

/**
Send data to the connected host
*/
int SocketManager::send_data(SOCKET socket, const char* message) // TODO: change the fact that if success - returns 2 (bytes of OK)
{
	char *packetOK = new char[OK_PACKET_SIZE];
	int result;

	// send size of messsage 
	string messageString(message);
	//string messageLength = to_string(messageString.length());
	string length = to_string(messageString.length());
	const char* messageLength = length.c_str();
	result = send(socket, messageLength, (int)strlen(messageLength), 0);
	if (result == SOCKET_ERROR) {
		closesocket(socket); // TODO - need it?
		throw new exception("send failed with error: %d\n", GetLastError());
		// WSACleanup()
	}

	// receive OK for size
	memset(packetOK, 0, OK_PACKET_SIZE);
	result = recv(socket, packetOK, OK_PACKET_SIZE, 0);//ok
	if (result == 0)
		throw new exception("Connection closed\n");
	else if (result < 0)
		throw new exception("recv failed with error: %d\n", WSAGetLastError());


	// send message
	result = send(socket, message, (int)strlen(message), 0);
	if (result == SOCKET_ERROR) {
		closesocket(socket); // TODO: need it? 
		throw new exception("send failed with error: %d\n", GetLastError());
		// WSACleanup()
	}

	// recieve OK for message
	memset(packetOK, 0, OK_PACKET_SIZE);
	result = recv(socket, packetOK, OK_PACKET_SIZE, 0);//ok
	if (result == 0)
		throw new exception("Connection closed\n");
	else if (result < 0)
		throw new exception("recv failed with error: %d\n", WSAGetLastError());
	
	return result;
}

/**
Receive data from the connected host
*/
int SocketManager::receive(SOCKET socket, char* buffer /*out*/, int bufferlen) // TODO: change the fact that if success returns 2 - bytes of sent OK
{
	int result;
	
	// get size of packet
	char sizeOfPacket[DEFAULT_BUFLEN];
	memset(sizeOfPacket, 0, DEFAULT_BUFLEN);
	
	result = recv(socket, sizeOfPacket, DEFAULT_BUFLEN, 0);
	if (result == 0)
		throw new exception("Connection closed\n");
	else if (result < 0)
		throw new exception("recv failed with error: %d\n", WSAGetLastError());

	// send OK for size
	result = send(socket, "ok", OK_PACKET_SIZE, 0);//ok
	if (result == SOCKET_ERROR) {
		closesocket(socket); // TODO: need it? 
		throw new exception("send failed with error: %d\n", GetLastError());
		// WSACleanup()
	}

	// receive message
	int sizePacket = atoi(sizeOfPacket);
	char *packet = new char[sizePacket];
	memset(packet, 0, sizePacket);

	result = recv(socket, packet, sizePacket, 0);

	if (result == 0)
		throw new exception("Connection closed\n");
	else if (result < 0 )
		throw new exception("recv failed with error: %d\n", WSAGetLastError());

	// send OK for receive
	result = send(socket, "ok", OK_PACKET_SIZE, 0);//ok
	if (result == SOCKET_ERROR) {
		closesocket(socket); // TODO: need it? 
		throw new exception("send failed with error: %d\n", GetLastError());
		// WSACleanup()
	}

	// save packet to buffer
	memset(buffer, 0, bufferlen);
	strncpy(buffer, packet, sizePacket);

	return result;
}