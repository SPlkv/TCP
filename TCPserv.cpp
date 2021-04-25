
#include <iostream>
#include <WS2tcpip.h>
#include <string>

#pragma comment (lib, "ws2_32.lib")

using namespace std;

void main()
{
	// Инициализировать winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	int wsOk = WSAStartup(ver, &wsData);
	if (wsOk != 0)
	{
		cerr << "Can't Initialize winsock! Quitting" << endl;
		return;
	}

	// Создание сокета
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET)
	{
		cerr << "Can't create a socket! Quitting" << endl;
		return;
	}


	// Привязываем ip-адрес и порт к сокету
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000);
	hint.sin_addr.S_un.S_addr = INADDR_ANY;  

	bind(listening, (sockaddr*)&hint, sizeof(hint));


	// Сообщаем Winsock, что сокет предназначен для прослушивания
	listen(listening, SOMAXCONN);

	// Ожидаем подключения
	sockaddr_in client;
	int clientSize = sizeof(client);

	SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);

	char host[NI_MAXHOST];   // Удаленное имя клиента
	char service[NI_MAXSERV]; // Сервис (то есть порт), к которому подключается клиент

	ZeroMemory(host, NI_MAXHOST);
	ZeroMemory(service, NI_MAXSERV);

	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
	{
		cout << host << " connected on port " << service << endl;
	}
	else
	{
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		cout << host << " connected on port " << ntohs(client.sin_port) << endl;
	}

	// Закрываем прослушивающий сокет
	closesocket(listening);

	// Цикл while: принять и отправить сообщение обратно клиенту
	char buf[4096];

	while (true)
	{
		ZeroMemory(buf, 4096);


		// Ждем, пока клиент отправит данные
		int bytesReceived = recv(clientSocket, buf, 4096, 0);
		if (bytesReceived == SOCKET_ERROR)
		{
			cerr << "Error in recv(). Quitting" << endl;
			break;
		}

		if (bytesReceived == 0)
		{
			cout << "Client disconnected " << endl;
			break;
		}

		cout << string(buf, 0, bytesReceived) << endl;

		// Эхо-сообщение обратно клиенту
		send(clientSocket, buf, bytesReceived + 1, 0);

	}

	// Закрываем сокет
	closesocket(clientSocket);

	// Очищаем winsock
	WSACleanup();

	system("pause");
}


