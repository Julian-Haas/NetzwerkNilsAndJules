#pragma comment (lib, "ws2_32.lib")
#pragma comment (lib, "iphlpapi.lib")

#include <cstdio>
#include <WinSock2.h>
#include <iphlpapi.h>
#include <WS2tcpip.h>
#include <string>
#include <vector>
#include <iostream>


//speichern der nutzer 
std::vector<std::vector<std::string>> user(10, std::vector<std::string>(2));

//speichern der tweets + nutzernamen 
std::vector<std::vector<std::string>> tweets;

//counter der nutzer
int index = 0;
std::string activeUser; 
//id des nutzers der gerade angemeldet ist
int userIndex = 0;
int userTweetID = 0; 
//tweet counter
int tweetCounter = 0;

void SendToClient(SOCKET i, std::string msg)
{
	char formatedAnwser[4096];
	strcpy_s(formatedAnwser, msg.c_str());
	send(i, formatedAnwser, sizeof(formatedAnwser), 0);
}

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

	int msgLenght = request[1] - '0';
	std::cout << request[1] <<"\n";
	std::string tweet = std::string(request + 2, msgLenght);
	return tweet; 
}

/*
  CheckForUserName in GetUserPost als security check verwenden  
*/
bool CheckForUserName(char request[]) 
{
	int nameLength = request[1] - '0'; //-> ASCII in INT _> eig. falsche l�sung 
	std::cout << "Laenge des Names: " << nameLength << "\n";
	activeUser = std::string(request + 2, nameLength);
	std::cout << "Username: " << activeUser << "\n";
	for (int i = 0; i < user.size(); i++)
	{
		userTweetID = i; 
		if (activeUser == user[i][0]) 
		{
			userIndex = i; //unn�tig 
			return true;
		}
	}
	user[index][0] = activeUser;
	return false;
}

std::string GetUserPosts(char request[])
{
	int usernameLenght = request[1] - '0';
	std::string username = std::string(request + 2, usernameLenght);
	std::string answer; 
	//speichern des Request Codes, der L�nge des Nutzernamens + nutzernamen gem�� protokoll
	char msgCode = char(DisplayHistoryOfUser_Server);
	answer += msgCode;
	answer += request[1];
	answer.append(username);
	for(int i = 0; i <= user.size()-1; i++)
	{
		if(username == user[i][0])
		{
			userTweetID = i; //falsch -> tweet id kann auch eine andere nummer als die nutzer id  sein...
			break; 
		}
	}
	//Safe Posts
	int endOfLoop = tweets[userTweetID].size() - 10;
	if (endOfLoop <= 0)
	{
		endOfLoop = 0;
	}

	//|103|TweetL�nge|Tweet| TweetL�nge | Tweet |
	for (int i = tweets[userTweetID].size() - 1; i >= endOfLoop; i--)
	{
		int lenghtOfPost = tweets[userTweetID][i].length();
		if (lenghtOfPost > 0)
		{
			char lenghtAscii = '0' + lenghtOfPost;
			answer += lenghtAscii;
			answer.append(tweets[userTweetID][i]);
		}
	}

	return answer;
}

void DisplayUserHistory(SOCKET i, char request[])
{
	std::string msg = GetUserPosts(request);
	std::cout << "Display User Tweets:\n" << msg << "\n";
	SendToClient(i, msg); 
}


void CheckUserNameForExistance(SOCKET i, char request[])
{
	printf("Handle Username: ");
	bool failed = CheckForUserName(request);
	std::string answer; 
	if (!failed) {

		answer = char(CheckUsernameForExistance_Server);
		answer.append(std::to_string(2)); // 2 = false;
		std::cout << answer[1] << "\n";
		SendToClient(i, answer); 
	}
	else {
		answer = char(CheckUsernameForExistance_Server);
		answer.append(std::to_string(1));
		std::cout << answer[1] << "\n";
		SendToClient(i, answer);
	}
}

void CheckPasswordForCorrectness(SOCKET i, char request[])
{

	/*
	|MSGCODE|L�ngeUsername|Username|L�nge von der request|stuff|
	*/
	std::string password;
	std::string answer; 
	int passwortStart = request[1] + 3 - '0';
	int passwortLenght = request[passwortStart - 1] - '0';
	password = GetPassword(request, passwortStart, passwortLenght);
	answer = "";
	if (user[userIndex][1] == password)
	{
		std::cout << "Correct passwort!" << "\n";
		answer = char(CheckPasswordForCorrectness_Server);
		answer.append(std::to_string(1));
		SendToClient(i, answer);
	}
	else
	{
		answer = char(CheckPasswordForCorrectness_Server);
		answer.append(std::to_string(2));
		SendToClient(i, answer);
	}
}

int GetUserTweetID(char request[])
{
	std::string answer;
	for (int i = 0; i <= user.size() - 1; i++)
	{
		if (activeUser == user[i][0])
		{
			return i; 
		}
	}
	return -1; 
}
int PostUserMessage(SOCKET i, char request[])
{
	std::string answer; 
	int userTweetID = GetUserTweetID(request);
	if(userTweetID == -1)
	{
		answer.clear();
		answer += char(PostAMessage_Server);
		char exitCode = '0' + 2; 
		answer += exitCode;
		SendToClient(i, answer);
		return-1; 
	}
	answer = ExtractTweet(request);

	if (userTweetID >= tweets.size()) {
		tweets.resize(userTweetID + 4); // Vergr��ere den Vektor, wenn n�tig
	}

	tweets[userTweetID].push_back(answer);
	tweetCounter++;
	answer.clear();
	answer += char(PostAMessage_Server);
	char exitCode = '0' + 2;
	answer += exitCode;
	SendToClient(i, answer);
	return 0; 
}

void FinishRegistration(SOCKET i, char request[])
{
	std::string password; 
	std::string answer; 
	int passwortStart = request[1] + 3 - '0';
	int passwortLenght = request[passwortStart - 1] - '0';
	password = GetPassword(request, passwortStart, passwortLenght);
	std::cout << "Password: " << password << std::endl;
	user[index][1] = password;
	answer = "";
	answer += char(RegisterUser_Server);
	answer += '1';
	SendToClient(i, answer);
	index++;
	userIndex = index;
}
void HandleIncomingRequest(bool& readingRequest, SOCKET i, char request[]) {

	int postWasSuccesFull;

	switch (request[0]) {
	case 1:
		CheckUserNameForExistance(i, request); 
		readingRequest = false;
		break;
	case 2:
		CheckPasswordForCorrectness(i, request); 
		readingRequest = false;
		break;
	case 3:
		DisplayUserHistory(i, request); 
		readingRequest = false;
		break;
	case 4:
		postWasSuccesFull = PostUserMessage(i, request);
		if(postWasSuccesFull == -1)
		{
			std::cout << "Failed to post the Message:\n Error code: -1! Username not found!\n";
		}
		readingRequest = false;
		break;
	case 5:
		FinishRegistration(i, request); 
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
							HandleIncomingRequest(readingRequest, i, request); 
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
