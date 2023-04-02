#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

#include <iostream>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <string>
#include <fstream>
#include <experimental/filesystem>

using namespace std;

void send_file(SOCKET* socket, const string& file_name)
{
	fstream file;
	file.open(file_name, ios_base::in | ios_base::binary);

	if (file.is_open())
	{
		int file_size = experimental::filesystem::file_size(file_name) + 1;

		char* bytes = new char[file_size];

		file.read((char*)bytes, file_size);

		cout << "size: " << file_size << endl;
		cout << "name: " << file_name << endl;

		send(*socket, to_string(file_size).c_str(), 16, 0);
		send(*socket, file_name.c_str(), 32, 0);
		send(*socket, bytes, file_size, 0);
		delete[] bytes;
	}
	else
		cout << "Error file open\n";

	file.close();

}

void ClientConnection(const char* _ip, int _port)
{
	cout << "\n___CLIENT___" << endl;

	WSADATA wsaData;
	WORD version = MAKEWORD(2, 2);// загружаем библиотеку
	int wsasErr = WSAStartup(version, &wsaData);

	bool bResult = (wsasErr != NO_ERROR);

	if (bResult)
	{
		cout << "DLL not found\nStatus";
		return;
	}

	SOCKET clientSocket = INVALID_SOCKET;
	clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	bResult = (clientSocket == INVALID_SOCKET);

	if (bResult)
	{
		cout << "socket failed! " << WSAGetLastError() << endl;
		WSACleanup();
		return;
	}

	in_addr ip_to_num;

	if (inet_pton(AF_INET, _ip, &ip_to_num) <= 0)
	{
		cout << "Error in IP" << endl;
		return;
	}

	sockaddr_in clientService;
	clientService.sin_family = AF_INET;
	clientService.sin_addr = ip_to_num;
	clientService.sin_port = htons(_port);

	bResult = (connect(clientSocket, (SOCKADDR*)&clientService, sizeof(clientService)) != SOCKET_ERROR);

	if (bResult)
	{
		cout << "connect" << endl;
		string path;
		cout << "file name: ";
		cin >> path;
		send_file(&clientSocket, path);
	
	}
	else
		cout << "No connnect\n";
}

int main()
{
	const char* ip = "127.0.0.1";
	int port = 27015;
	
	cout << " <ip> " << ip << " <port> " << port;

	ClientConnection(ip, port);

	system("pause");
	return 0;
}

