#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <string>
#include <fstream>
#include "Socket.h"
#include "WindowsSocket.h"
using namespace std;

#define DEFAULT_BUFLEN 1050
#define DEFAULT_PORT "5003"
#define DEFAULT_HOST "10.7.0.70"

std::ofstream openFile(char *file_path)
{
	std::ofstream f2(file_path, std::fstream::binary);
	return f2;
}


int main(int argc, char **argv)
{
	Socket clientSocket;
	WindowsSocket windowsSocket;
	SOCKET socket = INVALID_SOCKET;

	struct addrinfo *result = NULL, hints;
	char *sendbuf = "this is a test14";
	char recvbuf[DEFAULT_BUFLEN];
	int iResult, iResult2;
	int recvbuflen = DEFAULT_BUFLEN;
	std::ofstream clientFile;

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

		char * filePath = "C:\\Users\\admin\\Desktop\\CyberProject\\Client\\client_update_file.exe";
		clientFile = openFile(filePath);

		// Receive until the peer closes the connection
		do {
			try
			{
				char recvbuf[DEFAULT_BUFLEN * 4];
				iResult = clientSocket.receive(socket, recvbuf, recvbuflen*4);
				if (iResult > 0)
				{
					string buf = string(recvbuf);
					int length = strlen(recvbuf);
					printf("Bytes received: %d\n", iResult);
					cout << buf.substr(0, iResult);
					iResult2 = clientSocket.send_data(socket, "ok");
					for (int i = 0; i < iResult; i++)
					{
						clientFile << recvbuf[i];
					}
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


