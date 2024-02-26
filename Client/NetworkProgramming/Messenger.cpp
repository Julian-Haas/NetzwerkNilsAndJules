#pragma comment (lib, "ws2_32.lib")
#pragma comment (lib, "iphlpapi.lib")
#include <cstdio>
#include <conio.h>
#include <WinSock2.h>
#include <iphlpapi.h>
#include <WS2tcpip.h>
#include "Messenger.h"

enum Messenger::protocol
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

void Messenger::SetRequestCode(int requestCode)
{
    serverRequest = char(requestCode);
    //std::cout << serverRequest << "\n";
}

void Messenger::ExtendRequest(std::string appendedParameter)
{
    serverRequest.append(std::to_string(appendedParameter.length()));
    serverRequest.append(appendedParameter);
}

bool Messenger::CheckUsernameForExistance(std::string Username)
{
    SetRequestCode(CheckUsernameForExistance_Client);
    ExtendRequest(Username);
    SendToServer();
    return(WaitForServerResponse());
}

bool Messenger::CheckPasswordForCorrectness(std::string Username, std::string Password)
{
    SetRequestCode(CheckPasswordForCorrectness_Client);
    ExtendRequest(Username);
    ExtendRequest(Password);
    SendToServer();
    return(WaitForServerResponse());
}

void Messenger::DisplayHistoryOfUser(std::string username)
{
    SetRequestCode(DisplayHistoryOfUser_Client);
    ExtendRequest(username);
    SendToServer();
}

void Messenger::PostAMessage(std::string message)
{
    SetRequestCode(PostAMessage_Client);
    ExtendRequest(message);
    SendToServer();
}

void Messenger::ModePostAMessage()
{
    std::string message;
    while (true)
    {
        std::cout << "Please enter a message with maximum 280 characters which you want to post.\n";
        std::cin.ignore(10000, '\n');
        std::getline(std::cin, message);
        if (message.length() > 280)
        {
            std::cout << "Your message was too long.\n";
            continue;
        }
        PostAMessage(message);
        std::cout << "You posted this message: " << message << "\n";
        break;
    }
}

void Messenger::ModeUserSearch()
{
    std::string username;
    std::cout << "Please enter a username whose history you want to see.\n";
    std::cin >> username;
    if (CheckUsernameForExistance(username))
    {
        DisplayHistoryOfUser(username);
    }
    else
    {
        std::cout << "Following user is unknown: " << username << "\n";
        std::cout << "Going back to ModeAccountPage.\n";
    }
}

void Messenger::ModeAccountPage(std::string username)
{
    std::string chosenOption;
    bool correctInputFlag;
    while (chosenOption != "4")
    {
        correctInputFlag = false;
        std::cout << "Press 1 to show your own history, 2 to post a tweet, 3 to search for a user, or 4 to logout.\n";
        std::cin >> chosenOption;
        if (chosenOption.length() == 1)
        {
            switch (chosenOption[0])
            {
            case '1':
                DisplayHistoryOfUser(username);
                correctInputFlag = true;
                break;
            case '2':
                ModePostAMessage();
                correctInputFlag = true;
                break;
            case '3':
                ModeUserSearch();
                correctInputFlag = true;
                break;
            case '4':
                correctInputFlag = true;
                break;
            default:
                break;
            }
        }
        if (!correctInputFlag)
        {
            std::cout << "Wrong Input.\n";
        }
    }
    std::cout << "Logout was successful.\n";
}

void Messenger::Login()
{
    std::string enteredUsername;
    std::string enteredPassword;
    while (true)
    {
        while (true)
        {
            std::cout << "Please enter your username.\n";
            std::getline(std::cin, enteredUsername);
            SetRequestCode(CheckUsernameForExistance_Client);
            ExtendRequest(enteredUsername);
            SendToServer();
            if (!WaitForServerResponse())
            {
                std::cout << "This Username is already in use.\n";
                continue;
            }
            break;
        }
        std::cout << "Your username is " << enteredUsername << ".\n";
        std::cout << "Please enter your password.\n";
        std::getline(std::cin, enteredPassword);
        SetRequestCode(CheckPasswordForCorrectness_Client);
        CheckPasswordForCorrectness(enteredUsername, enteredPassword);
        if (WaitForServerResponse())
        {
            std::cout << "You logged in succesfully.\n";
            ModeAccountPage(enteredUsername);
            break;
        }
        else
        {
            std::cout << "Wrong password.\n";
            break;
        }
    }
}

void Messenger::Register()
{
    std::string enteredUsername;
    std::string enteredPassword;
    while (true)
    {
        while (true)
        {
            std::cout << "Please enter a username.\n";
            std::getline(std::cin, enteredUsername);
            if (CheckUsernameForExistance(enteredUsername))
            {
                std::cout << "This Username is already in use.\n";
                continue;
            }
            else
            {
                break;
            }
        }
        std::cout << "Your chosen username is " << enteredUsername << ".\n";
        std::cout << "Please enter a password.\n";
        std::getline(std::cin, enteredPassword);
        SetRequestCode(RegisterUser_Client);
        //std::cout << RegisterUser_Client << "\n";
        ExtendRequest(enteredUsername);
        ExtendRequest(enteredPassword);
        SendToServer();
        bool RegisterSuccessful = false;
        if (WaitForServerResponse())
        {
            std::cout << "You registered succesfully.\n";
            ModeAccountPage(enteredUsername);
            break;
        }
        else
        {
            std::cout << "Registration failed\n";
            break;
        }
        break;
    }
}

void Messenger::StartMessenger(int argc, char* argv[])
{
    WSAData d;
    if (WSAStartup(MAKEWORD(2, 2), &d))
    {
        printf("WinSocket failed to initialize\n");
        return;
    }
    if (argc < 3)
    {
        printf("usage tcp client [ip] [port]\n");
        return;
    }
    printf("Configuring remote address\n");
    addrinfo hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    addrinfo* server;
    if (getaddrinfo(argv[1], argv[2], &hints, &server))
    {
        fprintf(stderr, "getaddrinfo() failed. (%d)\n", WSAGetLastError());
        return;
    }
    printf("Remote adress is :\n");
    char adressBuffer[100];
    getnameinfo(server->ai_addr, server->ai_addrlen, adressBuffer, sizeof(adressBuffer), 0, 0, NI_NUMERICHOST);
    printf("%s\n", adressBuffer);
    printf("Creating socket...\n");
    serverSocket = socket(server->ai_family, server->ai_socktype, server->ai_protocol);
    if (serverSocket == INVALID_SOCKET)
    {
        fprintf(stderr, "socket(9 failed. %d)\n", WSAGetLastError());
        return;
    }
    printf("Connection to server...\n");
    if (connect(serverSocket, server->ai_addr, server->ai_addrlen))
    {
        fprintf(stderr, "connect() failed. (%d)\n", WSAGetLastError());
        return;
    }
    freeaddrinfo(server);
    printf("Connected!\n");
    printf("To send data, enter the text followed by enter \n");
    MainMenu();
}

bool Messenger::WaitForServerResponse()
{
    while (true)
    {
        fd_set reads;
        FD_ZERO(&reads);
        FD_SET(serverSocket, &reads);
        timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 100000;
        if (select(serverSocket + 1, &reads, 0, 0, &timeout) < 0)
        {
            fprintf(stderr, "select() failed. (%d)\n", WSAGetLastError());
            return false;
        }

        if (FD_ISSET(serverSocket, &reads))
        {
            char read[4096];
            int bytesReceived = recv(serverSocket, read, sizeof(read), 0);
            int val = 0; 
            switch (read[0])
            {
            case 101:
                val = read[1] - '0';
                return (val == 1);
                break;
            case 102:
                val = read[1] - '0';
                return (val == 1);
                break;
            case 103:
                DisplayReceivedHistory(read);
                return true;
                break;
            case 104:
                val = read[1] - '0';
                return (val == 1);
                break;
            case 105:
                val = read[1] - '0';
                return (val == 1);
                break;
            default:
                break;
            }
            printf("Received size: %d\n", sizeof(read));
            printf("Received: %s", read);
            if (bytesReceived < 1)
            {
                printf("Connection closed.\n");
                return false;
            }
        }
    }
}

void Messenger::SendToServer()
{
    strcpy_s(formattedRequest, serverRequest.c_str());
    send(serverSocket, formattedRequest, sizeof(formattedRequest), 0);
}

void Messenger::MainMenu()
{
    //std::string usaaaname = "Nils";
    //std::string paaasword = "1234";
    //SetRequestCode(RegisterUser_Client);
    //ExtendRequest(usaaaname);
    //ExtendRequest(paaasword);
    //SendToServer();
    // 54Nils41234
    //std::cout << "Keycode: " << char(serverRequest[0]) << "\n";
    std::cout << "Keycode: " << serverRequest << "\n";
    std::string chosenOption = "";
    while (true)
    {
        std::cout << "Please enter 1 to login or 2 to register\n";
        std::cin >> chosenOption;
        std::cin.ignore(10000, '\n');
        if (chosenOption == "1")
        {
            Login();
        }
        else if (chosenOption == "2")
        {
            Register();
        }
    }
}

void Messenger::DisplayReceivedHistory(char container[])
{
    //int AmountOfMessages = int(container[1]);
    //int LengthOfUsername = int(container[2]);
    //std::string username(container + 3, container + LengthOfUsername + 3);

    //int LengthOfMessage = int(container[LengthOfUsername + 4]) * 256 + int(container[LengthOfUsername + 4]);



}

//std::string usaaaname = "Nils";
//std::string paaasword = "1234";
//SetRequestCode(RegisterUser_Client);
//ExtendRequest(std::to_string(usaaaname.length()));
//ExtendRequest(usaaaname);
//ExtendRequest(std::to_string(paaasword.length()));
//ExtendRequest(paaasword);
//SendToServer();
//// 54Nils41234
//std::cout << "Keycode: " << serverRequest << "\n";