#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

#include <iostream>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <fstream>
#include <experimental/filesystem>
#include <string>

using namespace std;


void recv_file(SOCKET* socket)
{
	char file_size_str[1024];
	char file_name[32];

	recv(*socket, file_size_str, 16, 0);
	int file_size = atoi(file_size_str);
	char* bytes = new char[file_size];

	recv(*socket, file_name, 32, 0);

	fstream file;
	file.open(file_name, ios_base::out | ios_base::binary);

	cout << "size: " << file_size << endl;
	cout << "name: " << file_name << endl;

	if (file.is_open())
	{
		recv(*socket, bytes, file_size, 0);
		cout << "data: " << bytes << endl;

		file.write(bytes, file_size);
		cout << "ok save\n";
	}
	else
		cout << "Error file open\n";

	delete[] bytes;
	file.close();
}

void ServerConnection(PCSTR _ip, int _port) 
{
	SOCKET serverSocket;
	bool bResult;

	cout << "\n___SERVER___" << endl;

	WSADATA wsaData;
	WORD version = MAKEWORD(2, 2);
	int wsasErr = WSAStartup(version, &wsaData);

	bResult = (wsasErr != NO_ERROR);

	if (bResult)
	{
		cout << "WSAStartup failed with error:" << WSAGetLastError() << endl;
		cout << "Status:" << wsaData.szSystemStatus;
		return;
	}

	serverSocket = INVALID_SOCKET;
	serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	bResult = (serverSocket == INVALID_SOCKET);

	if (bResult)
	{
		cout << "socket failed with error: " << WSAGetLastError() << endl;
		WSACleanup();
		return;
	}

	in_addr ip_to_num;

	if (inet_pton(AF_INET, _ip, &ip_to_num) <= 0)
	{
		cout << "Error in IP" << endl;
		return;
	}

	sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_addr = ip_to_num;
	service.sin_port = htons(_port);

	bResult = (bind(serverSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR);

	if (bResult)
	{
		cout << "bind failed with error: " << WSAGetLastError() << endl;
		closesocket(serverSocket);
		WSACleanup();
		return;
	}

	bResult = (listen(serverSocket, 1) == SOCKET_ERROR);

	if (bResult)
	{
		cout << "listen failed with error: " << WSAGetLastError() << endl;
		closesocket(serverSocket);
		WSACleanup();
		return;
	}

	SOCKET acceptSocket;

	acceptSocket = accept(serverSocket, NULL, NULL);
	bResult = (acceptSocket != INVALID_SOCKET);

	if (bResult)
	{
		cout << "connect\n";
		recv_file(&acceptSocket);
	}
	else
		cout << "No connnect";
}

int main()
{
	PCSTR ip = "127.0.0.1";
	int port = 27015;
	cout << " <ip> " << ip << " <port> " << port;

	ServerConnection(ip, port);
	

	system("pause");
	return 0;
}

