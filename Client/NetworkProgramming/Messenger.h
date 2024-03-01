#pragma once
#include <WinSock2.h>
#include <iostream>
#include <string>

class Messenger
{
private:
		SOCKET serverSocket;
		std::string serverRequest;
		std::string nameOfActiveUser="";
		char formattedRequest[4096];
		char receivedMessage[4096];
		enum protocol;
		bool CheckUsernameForExistance(std::string Username);
		bool CheckPasswordForCorrectness(std::string Username, std::string Password);
		void DisplayHistoryOfUser(std::string username);
		void ModePostAMessage();
		void ModeUserSearch();
		void ModeAccountPage(std::string username);
		void Login();
		void Register();
		void MainMenu();
		bool WaitForServerResponse();
		void SendToServer();
		void ExtendRequest(std::string appendedParameter);
		void SetRequestCode(int requestCode);
		void DisplayReceivedHistory();
		void PostAMessage(std::string message);
		bool RegisterOnServer(std::string Username, std::string Password);
		void AddMessageLenght(std::string msg);
	public:
		void StartMessenger(int argc, char* argv[]);
};