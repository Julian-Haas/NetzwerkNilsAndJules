#pragma comment (lib, "ws2_32.lib")
#pragma comment (lib, "iphlpapi.lib")

#include <cstdio>
#include <WinSock2.h>
#include <iphlpapi.h>
#include <WS2tcpip.h>
#include <string>
#include <vector>
#include <iostream>

enum protocol
{
	CheckUsernameForExistance_Client = 1,
	CheckPasswordForCorrectness_Client = 2,
	DisplayHistoryOfUser_Client = 3,
	PostAMessage_Client = 4,
	RegisterUser_Client = 5,
	CheckUsernameForExistance_Server = 101,
	CheckPasswordForCorrectness_Server = 102,
	DisplayHistoryOfUser_Server = 103,
	PostAMessage_Server = 104,
	RegisterUser_Server = 105
};

std::string GetPassword(char request[], int start, int lenght)
{
	std::string password = ""; 
	for(int i = start; i < (start +lenght); i++)
	{
		password += request[i]; 
	}
	return password; 
}

std::string ExtractTweet(char request[])
{

	int tweetOffset = request[1] - '0'; 
	tweetOffset = tweetOffset + 2; 
	int msgLenght = request[tweetOffset - 1]; 
	std::string tweet = std::string(request + tweetOffset, msgLenght);
	return tweet; 
}

bool CheckForUserName(int* index, std::vector<std::vector<std::string>>* user, char request[], int* userIndex) {
	int nameLength = request[1] - '0';
	std::cout << "Laenge des Names: " << nameLength << "\n";
	std::string username = std::string(request + 2, nameLength);
	std::cout << "Username: " << username << "\n";
	bool uniqueUsername = true;
	for (int i = 0; i < *index; i++) {
		if (username == (*user)[i][0]) {
			(*userIndex) = i; 
			return true;
		}
	}
	(*user)[*index][0] = username;
	return false;
}

void HandleIncomingRequest(bool& readingRequest, SOCKET i, char request[], std::vector<std::vector<std::string>>* user, std::vector<std::vector<std::string>>* tweets, int* index, int* userIndex, int* tweetCounter) {
	std::cout << user << std::endl;
	std::cout << request << "\n";
	readingRequest = true;
	bool failed = false;
	int passwortStart;
	int passwortLenght;
	std::string userMsg = ""; 
	std::string password = " ";
	std::string anwser = " ";

	switch (request[0]) {
	case 1:
		printf("Handle Username: ");
		failed = CheckForUserName(index, user, request, userIndex);
		if (!failed) {
			anwser = char(CheckUsernameForExistance_Server);
			anwser.append(std::to_string(2));
			char formatedAnwser[4096];
			strcpy_s(formatedAnwser, anwser.c_str());
			std::cout << "name is valid!" << std::endl;
			send(i, formatedAnwser, 4096, 0);
		}
		else {
			anwser = char(CheckUsernameForExistance_Server);
			anwser.append(std::to_string(1));
			std::cout << anwser[1] << "\n";
			char formatedAnwser[4096];
			strcpy_s(formatedAnwser, anwser.c_str());
			send(i, formatedAnwser, 4096, 0);
		}
		readingRequest = false;
		break;
	case 2:
		passwortStart = request[1] + 3 - '0';
		passwortLenght = request[passwortStart - 1] - '0';
		password = GetPassword(request, passwortStart, passwortLenght);
		anwser = ""; 
		if((*user)[*userIndex][1] == password)
		{
			std::cout << "Correct passwort!" << "\n"; 
			anwser = char(CheckPasswordForCorrectness_Server);
			anwser.append(std::to_string(1));
			char formatedAnwser[4096];
			strcpy_s(formatedAnwser, anwser.c_str());
			send(i, formatedAnwser, 4096, 0);
		} else
		{
			anwser = char(CheckPasswordForCorrectness_Server);
			anwser.append(std::to_string(2));
			char formatedAnwser[4096];
			strcpy_s(formatedAnwser, anwser.c_str());
			send(i, formatedAnwser, 4096, 0);
		}
		readingRequest = false;
		break;
	case 3:
		printf("Handle User History: ");
		send(i, "Searching for your Browser History...disgusting...", 4096, 0);
		readingRequest = false;
		break;
	case 4:
		(*tweets)[(*userIndex)][(*tweetCounter)] = ExtractTweet(request);
		std::cout << (*tweets)[(*userIndex)][(*tweetCounter)] << std::endl; 
		(*tweetCounter)++; 
		anwser = "";
		anwser += char(PostAMessage_Server);
		anwser += '1';
		char formatedAnwser[4096];
		strcpy_s(formatedAnwser, anwser.c_str());
		send(i, formatedAnwser, 4096, 0);
		readingRequest = false;
		break;
	case 5:
		passwortStart = request[1] + 3 - '0';
		passwortLenght = request[passwortStart - 1] - '0';
		password = GetPassword(request, passwortStart, passwortLenght);
		std::cout << "Password: " << password << std::endl;
		(*user)[*index][1] = password;
		anwser = "";
		anwser += char(RegisterUser_Server);
		anwser += '1';
		char mformatedAnwser[4096];
		strcpy_s(mformatedAnwser, anwser.c_str());
		send(i, mformatedAnwser, 4096, 0);
		(*index)++;
		(*userIndex) = *index; 
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
	// pointer werden verwendet, damit die variablen die ganze die selben sind, und nicht beim verlassen einer methode gelöscht werden
	// --> Werte werden also erst gelöscht, wenn das Programm geschlossen wird. 

	//speichern der nutzer 
	std::vector<std::vector<std::string>> user(10, std::vector<std::string>(2));
	std::vector<std::vector<std::string>>* pUser = &user;

	//speichern der tweets + nutzernamen 
	std::vector<std::vector<std::string>> tweets(10, std::vector<std::string>(10));
	std::vector<std::vector<std::string>>* pTweets = &tweets;
	
	//counter der nutzer
	int index = 0; 
	int* pIndex = &index; 
	
	//id des nutzers der gerade angemeldet ist
	int userIndex = 0; 
	int* pUserIndex = &userIndex;
	
	//tweet counter
	int tweetCounter = 0; 
	int* pTweetCounter = &tweetCounter; 
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
							HandleIncomingRequest(readingRequest, i, request, pUser, pTweets,  pIndex, pUserIndex, pTweetCounter); 
						}
					}
					else 
					{
					    FD_CLR(i, &master);
					    closesocket(i);
					    printf("Closed socket\n");
					}
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
