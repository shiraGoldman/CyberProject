#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <string>
#include "Socket.h"
#include "WindowsSocket.h"
using namespace std;

#define DEFAULT_BUFLEN 1050
#define DEFAULT_PORT "5007"
#define DEFAULT_HOST "192.168.43.49"

int main(int argc, char **argv)
{
	Socket clientSocket;
	WindowsSocket windowsSocket;
	SOCKET socket = INVALID_SOCKET;

	struct addrinfo *result = NULL, hints;
	char *sendbuf = "this is a test14";
	char recvbuf[DEFAULT_BUFLEN];
	int iResult;
	int recvbuflen = DEFAULT_BUFLEN;

	// Initialize Winsock
	try
	{
		windowsSocket.startup();
		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		// Resolve the server address and port
		iResult = getaddrinfo(DEFAULT_HOST, DEFAULT_PORT, &hints, &result);
		if (iResult != 0) {
			throw new exception("getaddrinfo failed with error: %d\n", iResult);
		}

		clientSocket.setAddrInfo(result);
		
		// Attempt to connect to an address until one succeeds
		bool connectResult = false;
		while (true) // handle timeout!
		{
			// Create a SOCKET for connecting to server
			socket = clientSocket.create();

			// Connect to server.
			connectResult = clientSocket.conn(socket);
			if (connectResult == SOCKET_ERROR) {
				clientSocket.close(socket);
				socket = INVALID_SOCKET;
				continue;
			}
			break;
		}

		freeaddrinfo(result);

		/*if (ConnectSocket == INVALID_SOCKET) {
			printf("Unable to connect to server!\n");
			WSACleanup();
			return 1;
		}*/

		// Send an initial buffer
		iResult = clientSocket.send_data(socket, sendbuf);

		printf("Bytes Sent: %ld\n", iResult);

		// shutdown the connection since no more data will be sent
		////iResult = shutdown(ConnectSocket, SD_SEND);
		//if (iResult == SOCKET_ERROR) {
		//	printf("shutdown failed with error: %d\n", WSAGetLastError());
		//	closesocket(ConnectSocket);
		//	WSACleanup();
		//	return 1;
		//}

		// Receive until the peer closes the connection
		do {
			try
			{
				char recvbuf[DEFAULT_BUFLEN];
				iResult = clientSocket.receive(socket, recvbuf, recvbuflen);
				if (iResult > 0)
				{
					string buf = string(recvbuf);
					int meessageLen = atoi((buf.substr(strlen(recvbuf) - 2)).c_str());
					printf("Bytes received: %d\n", iResult);
					cout << buf.substr(0, meessageLen -1);
				}
			}
			catch (exception ex)
			{
				cout << ex.what();
				continue;
			}

		} while (iResult > 0);

		// cleanup
		clientSocket.close(socket);
		windowsSocket.cleanup();
		return 0;
	}
	catch (exception ex)
	{
		cout << ex.what();
		windowsSocket.cleanup();
		return 1;
	}
}


