#pragma once
#include <WinSock2.h>
#include <iostream>

class Messenger
{
	private:
		SOCKET serverSocket;
		// request
		// requestlength
		enum protocol;
		bool CheckUsernameForExistance(std::string Username);
		bool CheckPasswordForCorrectness(std::string Username, std::string Password);
		void DisplayHistoryOfUser(std::string username);
		void PostMode();
		void UserSearchMode();
		void AccountPage(std::string username);
		void Login();
		void Register();
		void MainMenu();
		void WaitForServer();
		void SendToServer();
	public:
		void StartMessenger(int argc, char* argv[]);
};