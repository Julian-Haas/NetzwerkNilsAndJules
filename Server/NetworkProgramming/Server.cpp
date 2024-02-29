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

std::string AddMessageLenght(std::string msg, int length)
{
	char val1;
	char val2;
	if (length > 255)
	{
		int rest = length - 255;
		val1 = 255;
		val2 = rest;
		msg += val1;
		msg += val2;
	}
	else
	{
		val1 = length;
		val2 = 0;
		msg += val1;
		msg += val2;
	}
	return msg;
}

int GetStringLenght(char request[], int start)
{
	return request[start] + request[start + 1];
}
void SendToClient(SOCKET i, std::string msg)
{
	char formatedAnwser[4096];
	strcpy_s(formatedAnwser, msg.c_str());
	std::cout << formatedAnwser << "\n"; 
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

	int msgLenght = GetStringLenght(request, 1); 
	std::string tweet = std::string(request + 3, msgLenght);
	return tweet; 
}

/*
  CheckForUserName in GetUserPost als security check verwenden  
*/
bool CheckForUserName(char request[]) 
{
	int nameLength = GetStringLenght(request, 1);
	std::cout << "Laenge des Names: " << nameLength << "\n";
	activeUser = std::string(request + 3, nameLength);
	std::cout << "Username: " << activeUser << "\n";
	for (int i = 0; i < user.size(); i++)
	{
		userTweetID = i; 
		if (activeUser == user[i][0]) 
		{
			userIndex = i;
			return true;
		}
	}
	user[index][0] = activeUser;
	return false;
}

std::string GetUserPosts(char request[])
{
	int usernameLenght =GetStringLenght(request, 1);
	std::string username = std::string(request + 3, usernameLenght);
	std::string answer; 
	//speichern des Request Codes, der Länge des Nutzernamens + nutzernamen gemäß protokoll
	char msgCode = DisplayHistoryOfUser_Server;
	answer += msgCode;

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
	int postAmount; 
	if (endOfLoop <= 0)
	{
		endOfLoop = 0;
		postAmount = tweets[userTweetID].size(); 
	} else 
	{
		postAmount = 10; 
	}
	char postAmountChar = postAmount; 
	answer += postAmountChar; 
	answer = AddMessageLenght(answer, usernameLenght);
	answer.append(username);
	//|103|TweetLänge|Tweet| TweetLänge | Tweet |
	for (int i = tweets[userTweetID].size() - 1; i >= endOfLoop; i--)
	{
		int lenghtOfPost = tweets[userTweetID][i].length();
		if (lenghtOfPost > 0)
		{
			answer += AddMessageLenght(answer, lenghtOfPost);
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

		answer = CheckUsernameForExistance_Server;
		char exitCode = 2; 
		answer += exitCode; 
		std::cout << answer[1] << "\n";
		SendToClient(i, answer); 
	}
	else {
		answer = CheckUsernameForExistance_Server;
		char exitCode = 1;
		answer += exitCode;
		//std::cout << answer[1] << "\n";
		SendToClient(i, answer);
	}
}

void CheckPasswordForCorrectness(SOCKET i, char request[])
{

	/*
	|MSGCODE|LängeUsername|Username|Länge von der request|stuff|
	*/
	std::string password;
	std::string answer; 
	int passwortStart = GetStringLenght(request, 1) + 5;
	int passwortLenght = GetStringLenght(request, passwortStart-1);
	password = GetPassword(request, passwortStart, passwortLenght);
	answer = "";
	if (user[userIndex][1] == password)
	{
		answer = CheckPasswordForCorrectness_Server;
		std::cout << "Correct passwort!" << "\n";
		char returnCode = 1;
		answer += returnCode;
		SendToClient(i, answer);
	}
	else
	{
		answer = CheckPasswordForCorrectness_Server;
		char returnCode = 2; 
		answer += returnCode; 
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
		answer += PostAMessage_Server;
		char exitCode = 2; 
		answer += exitCode;
		SendToClient(i, answer);
		return-1; 
	}
	answer = ExtractTweet(request);

	if (userTweetID >= tweets.size()) {
		tweets.resize(userTweetID + 4); // Vergrößere den Vektor, wenn nötig
	}

	tweets[userTweetID].push_back(answer);
	tweetCounter++;
	answer.clear();
	answer += PostAMessage_Server;
	char exitCode = 2;
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
	answer += RegisterUser_Server;
	answer += 1;
	SendToClient(i, answer);
	index++;
	userIndex = index;
}
void HandleIncomingRequest(bool& readingRequest, SOCKET i, char request[]) {

	int postWasSuccesFull;
	std::cout << request << "\n"; 
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
	unsigned char testArr[4096];
	unsigned char code = CheckPasswordForCorrectness_Server;
	std::string test;
	std::string msg;
	test.append("Ganz viele buchstaben, damit der chit hier ordentlich getestet werden kann. \n Deswegen schreibe ích hier ganz viel Bullshit rein.\n Ganz viele buchstaben, damit der chit hier ordentlich getestet werden kann. \n Deswegen schreibe ích hier ganz viel Bullshit rein.");
	int length = test.length();
	
	msg += code;
	memcpy(testArr, msg.data(), msg.size());
	int lengthOfMessage; 
	lengthOfMessage = testArr[1] + testArr[2]; 


	std::cout << int(testArr[0]) << "\n" << int(testArr[1]) << "\n" << int(testArr[2]) << "\n";
	std::cout << "real lenght: \n" << length << "\n" << "Calculated lenght: \n" << lengthOfMessage << "\n";

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
