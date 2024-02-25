#pragma comment (lib, "ws2_32.lib")
#pragma comment (lib, "iphlpapi.lib")

#include <cstdio>
#include <WinSock2.h>
#include <iphlpapi.h>
#include <WS2tcpip.h>
#include <string>
#include <vector>
#include <iostream>

bool HandleRegistration(int& index, std::vector<std::vector<std::string>>& user, char request[])
{
	int nameLength = request[1] - '0';
	std::cout << "Länge des Names: " << nameLength << "\n";
	std::string username(request + 2, nameLength); // Starte ab dem dritten Zeichen (Index 2) und lese nameLength Zeichen
	std::cout << "Username: " << username << "\n";

	int passwordStart = nameLength + 2; // Start des Passworts: 2 für den Request-Code und die Benutzerlänge, plus 1 für das Nullzeichen
	int passwordLength = request[passwordStart] - '0';
	std::string password(request[passwordStart + 1], passwordLength); // Starte ab dem Passwortstart + 1, um das Nullzeichen zu überspringen
	std::cout << "Password: " << password << "\n";

	//bool uniqueUsername = true;
	//for (int i = 0; i < index; i++)
	//{
	//	if (user[i][0] != "")
	//	{
	//		if (username == user[i][0])
	//		{
	//			uniqueUsername = false;
	//			break;
	//		}
	//	}
	//}

		std::vector<std::string> newUser;
		newUser.push_back(username);
		newUser.push_back(password);
		user.push_back(newUser);
		index++;
		return true; 
}


void HandleIncomingRequest(bool& readingRequest, SOCKET i, char request[], std::vector<std::vector<std::string>> user, int index)
{
	readingRequest = true;
	bool succes = NULL;
	switch (request[0])
	{
	case 5:
		printf("Handle Username: ");
		succes = HandleRegistration(index, user, request);
			if(succes != NULL)
			{
				if (succes)
				{
					send(i, "succes...", 4096, 0);
					break;
				}
				else
				{
					send(i, "failed...", 4096, 0);
					break;
				}
			}
		readingRequest = false;
		break;
	case '2':
		printf("Handle Passwort: ");
		send(i, "Checking if passwort is correct...", 4096, 0);
		readingRequest = false;
		break;
	case '3':
		printf("Handle User History: ");
		send(i, "Searching for your Browser History...disgusting...", 4096, 0);
		readingRequest = false;
		break;
	case '4':
		printf("tweet posted \n");
		send(i, "tweet posted", 4096, 0);
		readingRequest = false;
		break;
	default:
		printf("unhandled request");
		readingRequest = false;
		break;
	}
}

int main(int argc, char* argv[])
{
	//das hier später raus löschen und in klasse einbauen
	std::vector<std::vector<std::string>> user; 
	int index = 0; 
	
	//server set up
	WSAData d;
	bool readingRequest = false;
	if (WSAStartup(MAKEWORD(2, 2), &d))
	{
		printf("WinSocket failed to initialize\n");
		return -1;
	}
	printf("Configuring local ip address\n");
	addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	addrinfo* bindAddress;
	getaddrinfo(0, "8080", &hints, &bindAddress);
	printf("Creating listener socket\n");
	SOCKET listenerSocket;
	listenerSocket = socket(bindAddress->ai_family, bindAddress->ai_socktype, bindAddress->ai_protocol);
	if (listenerSocket == INVALID_SOCKET)
	{
		fprintf(stderr, "socket() failed. (%d)\n", WSAGetLastError());
		return -1;
	}
	u_long mode = 1; // 
	ioctlsocket(listenerSocket, FIONBIO, &mode); // makro das den listener zu nicht blockierendem makro macht
	printf("Binding address to socket\n");
	if (bind(listenerSocket, bindAddress->ai_addr, bindAddress->ai_addrlen))
	{
		fprintf(stderr, "bind() failed. (%d)\n", WSAGetLastError());
		return -1;
	}
	freeaddrinfo(bindAddress);
	printf("Listening...\n");
	if (listen(listenerSocket, 10) < 0)
	{
		fprintf(stderr, "listen() failed. (%d)\n", WSAGetLastError());
		return -1;
	}
	fd_set master;
	FD_ZERO(&master);
	FD_SET(listenerSocket, &master);
	SOCKET maxSocket = listenerSocket;
	//server loop
	while (true)
	{
		fd_set reads = master;
		if (select(maxSocket + 1, &reads, 0, 0, 0) < 0)
		{
			fprintf(stderr, "select() failed. (%d)\n", WSAGetLastError());
			return -1;
		}
		SOCKET i;
		for (i = 0; i <= maxSocket; i++)
		{
			if (FD_ISSET(i, &reads))
			{
				if (i == listenerSocket)
				{
					sockaddr_storage client;
					socklen_t clientLegth = sizeof(client);
					SOCKET clientSocket = accept(listenerSocket, reinterpret_cast<sockaddr*>(&client), &clientLegth);

					if (clientSocket == INVALID_SOCKET)
					{
						fprintf(stderr, "select() failed. (%d)\n", WSAGetLastError());
						return -1;
					}
					FD_SET(clientSocket, &master);
					if (clientSocket > maxSocket)
					{
						maxSocket = clientSocket;
					}
					char adressBuffer[100];
					getnameinfo(reinterpret_cast<sockaddr*>(&client), clientLegth, adressBuffer, sizeof(adressBuffer), 0, 0, NI_NUMERICHOST);
					printf("New connection from: %s\n", adressBuffer);
				}
				else
				{
					if (readingRequest == false)
					{
						printf("Reading request...\n");
						char request[4096];
						int bytesReceived = recv(i, request, sizeof(request), 0);
						if(bytesReceived > 1)
						{
							readingRequest = true;
							HandleIncomingRequest(readingRequest, i, request, user, index); 
						}
					}

					//if (bytesReceived < 1)
					//{
					//    FD_CLR(i, &master);
					//    closesocket(i);
					//    printf("Closed socket\n");
					//}
					//else
					//{
					//    for (int j = 0; j < bytesReceived; ++j)
					//    {
					//        request[j] = toupper(request[j]);
					//    }
					//    printf("Sending: %s\n", request);
					//    int bytesSent = send(i, request, sizeof(request), 0);
					//    printf("Sending %d bytes\n", bytesSent);
					//}
						 //FD_CLR(i, &master);
						 //closesocket(i);
				}
			}

		}
	}
	closesocket(listenerSocket);
	WSACleanup();
	return 0;
}
