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
char request[4096];

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

int GetStringLenght(int start)
{
	return request[start] + request[start + 1];
}
void SendToClient(SOCKET i, std::string msg)
{
	char formatedAnswer[4096];
	memcpy(formatedAnswer, msg.data(), msg.size());
	formatedAnswer[msg.size()] = '\0';

	if (formatedAnswer[0] == 103)
	{
		for (char c : formatedAnswer)
		{
			std::cout << int(c) << std::endl;
		}
	}

	send(i, formatedAnswer, msg.size() + 1, 0); // Übergeben Sie die tatsächlich kopierte Datenlänge
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

std::string GetPassword(int start, int lenght)
{
	std::string password = ""; 
	for(int i = start; i < (start +lenght); i++)
	{
		password += request[i]; 
	}
	return password; 
}

std::string ExtractTweet()
{

	int NameLenght = GetStringLenght(1); 
    //1 + 2(NameLenght) + namelenght + 2(tweetLenght)  
	int tweetLenghtPos = NameLenght + 3;
	int tweetLenght = GetStringLenght(tweetLenghtPos);
	std::string tweet = std::string(request + NameLenght + 5, tweetLenght);
	return tweet; 
}

/*
  CheckForUserName in GetUserPost als security check verwenden  
*/
bool CheckForUserName()
{
	int nameLength = GetStringLenght(1);
	//|1|4|0|116|101|115|116
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

std::string GetUserPosts()
{
	int usernameLenght = GetStringLenght(1);
	std::string username = std::string(request + 3, usernameLenght);
	std::string answer; 
	//speichern des Request Codes, der Länge des Nutzernamens + nutzernamen gemäß protokoll
	char msgCode = char(DisplayHistoryOfUser_Server);
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
		std::string temp = tweets[userTweetID][i];
		std::cout << temp << "\n";

		int lenghtOfPost = (tweets[userTweetID][i].length());
		//std::cout << lenghtOfPost << "\n";
		if (lenghtOfPost > 0)
		{
			answer = AddMessageLenght(answer, lenghtOfPost);
			answer.append(tweets[userTweetID][i]);
		}
	}
	return answer;
}

void DisplayUserHistory(SOCKET i)
{
	std::string msg = GetUserPosts();
	SendToClient(i, msg); 
}


void CheckUserNameForExistance(SOCKET i)
{
	printf("Handle Username: ");
	bool failed = CheckForUserName();
	std::string answer; 
	if (!failed) {

		answer = char(CheckUsernameForExistance_Server);
		char exitCode = char(2); 
		answer += exitCode; 
		std::cout << answer[1] << "\n";
		SendToClient(i, answer); 
	}
	else {
		answer = char(CheckUsernameForExistance_Server);
		char exitCode = char(1);
		answer += exitCode;
		//std::cout << answer[1] << "\n";
		SendToClient(i, answer);
	}
	return; 
}

void CheckPasswordForCorrectness(SOCKET i)
{
	/*
	|MSGCODE|LängeUsername|Username|Länge von der request|stuff|
	*/
	std::string password;
	std::string answer; 
	int passwortStart = GetStringLenght(1) + 5;
	int passwortLenght = GetStringLenght(passwortStart-2);
	password = GetPassword(passwortStart, passwortLenght);
	answer = "";
	if (user[userIndex][1] == password)
	{
		answer = char(CheckPasswordForCorrectness_Server);
		std::cout << "Correct passwort!" << "\n";
		char returnCode = char(1);
		answer += returnCode;
		SendToClient(i, answer);
	}
	else
	{
		answer = char(CheckPasswordForCorrectness_Server);
		char returnCode = char(2); 
		answer += returnCode; 
		SendToClient(i, answer);
	}
}

int GetUserTweetID()
{
	std::string answer;
	int usernameLenght = GetStringLenght(1);
	std::string username = std::string(request + 3, usernameLenght); 
	for (int i = 0; i <= user.size() - 1; i++)
	{
		if (username == user[i][0])
		{
			return i; 
		}
	}
	return -1; 
}
int PostUserMessage(SOCKET i)
{
	std::string answer; 
	int userTweetID = GetUserTweetID();
	if(userTweetID == -1)
	{
		answer.clear();
		answer += PostAMessage_Server;
		char exitCode = 2; 
		answer += exitCode;
		SendToClient(i, answer);
		return-1; 
	}
	answer = ExtractTweet();

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

void FinishRegistration(SOCKET i)
{
	std::string password; 
	std::string answer; 
	//int passwortStart = request[1] + 3 - '0';
	//int passwortLenght = request[passwortStart - 1] - '0';
	int passwortStart = GetStringLenght(1) + 5;
	int passwortLenght = GetStringLenght(passwortStart - 2);
	password = GetPassword(passwortStart, passwortLenght);
	std::cout << "Password: " << password << std::endl;
	user[index][1] = password;
	answer = "";
	answer += RegisterUser_Server;
	answer += 1;
	SendToClient(i, answer);
	index++;
	userIndex = index;
}
void HandleIncomingRequest(bool& readingRequest, SOCKET i) {

	int postWasSuccesFull;
	std::string temp = request; 
	for(int x = 0; x < 30; x++)
	{
		std::cout << "Inhalt der Anfrage: \n" << int(request[x]) << "\n";
	}

	switch (request[0]) {
	case 1:
		CheckUserNameForExistance(i); 
		readingRequest = false;
		break;
	case 2:
		CheckPasswordForCorrectness(i); 
		readingRequest = false;
		break;
	case 3:
		DisplayUserHistory(i);
		break;
	case 4:
		postWasSuccesFull = PostUserMessage(i);
		if(postWasSuccesFull == -1)
		{
			std::cout << "Failed to post the Message:\n Error code: -1! Username not found!\n";
		}
		readingRequest = false;
		break;
	case 5:
		FinishRegistration(i); 
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
	char testArr[4096];
	char code = CheckPasswordForCorrectness_Server;
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
					int bytesReceived = recv(i, request, sizeof(request), 0);
					if (bytesReceived > 0) {
						HandleIncomingRequest(readingRequest, i); 
					}
					else if (bytesReceived == 0) {
						// Verbindung geschlossen
						FD_CLR(i, &master);
						closesocket(i);
					}
					else {
						// Fehler beim Empfangen von Daten
						fprintf(stderr, "recv() failed. (%d)\n", WSAGetLastError());
						FD_CLR(i, &master);
						closesocket(i);
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
