#pragma comment (lib, "ws2_32.lib")
#pragma comment (lib, "iphlpapi.lib")
#include <cstdio>
#include <conio.h>
#include <WinSock2.h>
#include <iphlpapi.h>
#include <WS2tcpip.h>
#include <sstream>
#include <iomanip>
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

void Messenger::AddMessageLenght(std::string msg)
{
    int length = msg.length();

    char val1;
    char val2;
    if (length > 255)
    {
        int rest = length - 255;
        val1 = char(255);
        val2 = char(rest);
        serverRequest += val1;
        serverRequest += val2;
    }
    else
    {
        val1 = char(length);
        val2 = char(0);
        serverRequest += val1;
        serverRequest += val2;
    }
}

int GetStringLenght(char request[], int start)
{
    char blaa[5];
    blaa[0] = request[start];
    blaa[1] = request[start + 1];
    blaa[2] = unsigned char(blaa[0]);
    blaa[3] = unsigned char(blaa[1]);
    blaa[4] = unsigned char(blaa[0]) + unsigned char(blaa[1]);
    for (int i = 0; i < 5; i++)
    {
        std::cout << unsigned int(blaa[i]) << "\n";
    }
    return request[start] + request[start + 1];
}

void Messenger::SetRequestCode(int requestCode)
{
    serverRequest = char(requestCode);
    //std::cout << serverRequest << "\n";
}

void Messenger::ExtendRequest(std::string appendedParameter)
{
    AddMessageLenght(appendedParameter);
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

bool Messenger::RegisterOnServer(std::string Username, std::string Password)
{
    SetRequestCode(RegisterUser_Client);
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
    WaitForServerResponse();
}

void Messenger::PostAMessage(std::string message)
{
    SetRequestCode(PostAMessage_Client);
    ExtendRequest(nameOfActiveUser);
    ExtendRequest(message);
    SendToServer();
    WaitForServerResponse();
}

void Messenger::ModePostAMessage()
{
    std::string message;
    while (true)
    {
        std::cout << "Please enter a message with maximum 280 characters which you want to post.\n";
        std::cin.ignore(10000, '\n');
        std::getline(std::cin, message);
        ////char test = 0;
        //char test = '\0';
        if (message.length() > 280)
        {
            std::cout << "Your message was too long.\n";
            continue;
        }
        //message += test;
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
    std::cout << "You want to see the History of: \n" << username << "\n";
    CheckUsernameForExistance(username);
    bool doesUserExist = CheckUsernameForExistance(username);
    std::cout << "The Request to the server gave back: \n" << doesUserExist << "\n";
    if (doesUserExist)
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
            if (!CheckUsernameForExistance(enteredUsername))
            {
                std::cout << "This Username does not exist.\n";
                continue;
            }
            break;
        }
        std::cout << "Your username is " << enteredUsername << ".\n";
        std::cout << "Please enter your password.\n";
        std::getline(std::cin, enteredPassword);
        if (CheckPasswordForCorrectness(enteredUsername, enteredPassword))
        {
            std::cout << "You logged in succesfully.\n";
            nameOfActiveUser = enteredUsername;
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
        if (RegisterOnServer(enteredUsername, enteredPassword))
        {
            std::cout << "You registered succesfully.\n";
            nameOfActiveUser = enteredUsername;
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

            int bytesReceived = recv(serverSocket, receivedMessage, sizeof(receivedMessage), 0);
            //std::cout << read << std:: endl;
            //int i = -1;
            //for (char c : read)
            //{
            //    std::cout << int(c); //<< std::endl;
            //}
            //while (true)
            //{
            //    i++;
            //    //if (read[i] == 0) break;
            //    std::cout << int(read[i]); //<< std::endl;

            //}
            int val = 0; 
            switch (receivedMessage[0])
            {
            case 101:
                val = receivedMessage[1];
                return (val == 1);
                break;
            case 102:
                val = receivedMessage[1];
                return (val == 1);
                break;
            case 103:
                DisplayReceivedHistory();
                return true;
                break;
            case 104:
                val = receivedMessage[1];
                return (val == 1);
                break;
            case 105:
                val = receivedMessage[1];
                return (val == 1);
                break;
            default:
                break;
            }
            printf("Received size: %d\n", sizeof(receivedMessage));
            printf("Received: %s", receivedMessage);
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

    //strcpy_s(formattedRequest, serverRequest.c_str());
    memcpy(formattedRequest, serverRequest.data(), serverRequest.size());
    //if (formattedRequest[0] == 4)
    //{
    //    for (char c : formattedRequest)
    //    {
    //        std::cout << int(c) << std::endl;

    //    }
    //}

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
        std::cout << "Please enter 1 to login or 2 to register.\n";
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

void Messenger::DisplayReceivedHistory()
{
    //for (int i = 0; i < 40; i++)
    //{
    //    std::cout << int(receivedMessage[i]) << std::endl;
    //}
    //std::cout << std::endl;
    int amountOfMessages = receivedMessage[1];
    int positionToRead = 2;
    int lengthOfUsername = GetStringLenght(receivedMessage, positionToRead);
    positionToRead += 2;
    std::string username = std::string(receivedMessage + positionToRead, lengthOfUsername);
    positionToRead += lengthOfUsername;
    std::cout << "Here is the History of the User \n" << username << "\n";

    for (int i = 0; i < amountOfMessages; i++)
    {
        std::cout << "Start of loop of loop. amountOfMessages: " << amountOfMessages << "    i: " << i << "    positionToRead: " << positionToRead << std::endl;
        int timestamp = 0;
        for (int j = 0; j < 4; j++)
        {
            timestamp += unsigned char(receivedMessage[positionToRead]) * std::pow(256, 3 - j);
            positionToRead++;
        }
        std::time_t formattedTimestamp = static_cast<time_t>(timestamp);
        std::tm timeinfo1;
        localtime_s(&timeinfo1, &formattedTimestamp);
        std::stringstream ss;
        ss << std::put_time(&timeinfo1, "%d-%m-%Y %H:%M:%S");
        std::string timeString = ss.str();
        std::cout << "This Message was posted: " << timeString << std::endl;
        int lengthOfMessage = GetStringLenght(receivedMessage, positionToRead);
        positionToRead += 2;
        std::string message = std::string(receivedMessage+ positionToRead, lengthOfMessage);
        positionToRead += lengthOfMessage;
        std::cout << message << "\n";
        std::cout << "End of loop. amountOfMessages: " << amountOfMessages << "i: " << i << "positionToRead: " << positionToRead << std::endl;
    }
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