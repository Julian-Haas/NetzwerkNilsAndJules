#pragma once
#include <WinSock2.h>
#include <iostream>
#include<vector>
#include <string>
class Server
{
	private: 
		//speichern der nutzer 
		std::vector<std::vector<std::string>> user;
		std::vector<std::vector<int>> postTimes;
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

		//member functions: 
		std::string AddMessageLenght(std::string msg, int length); 
		int GetStringLenght(int start); 
		void SendToClient(SOCKET i, std::string msg); 
		std::string GetPassword(int start, int lenght); 
		std::string ExtractTweet(); 
		bool CheckForUserName(); 
		std::string GetUserPosts();
		void DisplayUserHistory(SOCKET i);
		void CheckUserNameForExistance(SOCKET i); 
		void CheckPasswordForCorrectness(SOCKET i); 
		int GetUserTweetID(); 
		int PostUserMessage(SOCKET i); 
		void FinishRegistration(SOCKET i); 
		void HandleIncomingRequest(bool& readingRequest, SOCKET i); 
		//protocol enum 
		enum protocol; 

public:
	Server(std::vector<std::vector<std::string>> nutzer); 
	int InitServer(int argc, char* argv[]);

};