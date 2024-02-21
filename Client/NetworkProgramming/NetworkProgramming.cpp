#pragma comment (lib, "ws2_32.lib")
#pragma comment (lib, "iphlpapi.lib")

#include <cstdio>
#include <conio.h>
#include <WinSock2.h>
#include <iphlpapi.h>
#include <WS2tcpip.h>
#include <iostream>
#include <string.h>

SOCKET globalSocket;

bool CheckUsernameForExistance(std::string Username)
{
    // prüfe username
    std::cout << "Error: Server is not implemented yet, returned a default value.\n";
    return true;
}

bool CheckPassword(std::string Password)
{
    // prüfe password
    std::cout << "Error: Server is not implemented yet, returned a default value.\n";
    return true;
}

void DisplayHistoryOfUser(std::string username)
{
    //retrieve data and display it
    //send(globalSocket, constMessage, sizeof(constMessage), 0); // Username noch mitgeben
    std::cout << "This is a placepolder for the history of " << username << ".\n";
}

void PostMode() // wie besser? klasse?
{
    std::string message = "";
    while (true)
    {
        std::cout << "Please enter a message with maximum 280 characters which you want to post.\n";
        std::cin >> message;
        if (message.length() > 280)
        {
            std::cout << "Your message was too long.\n";
            continue;
        }
        const char* constMessage = message.c_str();
        send(globalSocket, constMessage, sizeof(constMessage), 0); // Username noch mitgeben
        //nachricht auf dem server speichern mit uhrzeit und username
        std::cout << "You posted this message: " << message << "\n";
        break;
    }
}

void UserSearchMode()
{
    std::string username = "";
    while (true)
    {
        std::cout << "Please enter a username whose history you want to see.\n";
        std::cin >> username;
        if (CheckUsernameForExistance(username))
        {
            DisplayHistoryOfUser(username);
            break;
        }
        std::cout << "Following user is unknown: " << username << "\n";
        break;
    }
}


void AccountPage(std::string username)
{
    std::string chosenOption = "";
    while (chosenOption != "4")
    {
        std::cout << "Press 1 to show your own history, 2 to post a tweet, 3 to search for a user, or 4 to logout.\n";
        std::cin >> chosenOption;
        if (chosenOption.length() == 1)
        {
            switch (chosenOption[0])
            {
                case '1':
                    DisplayHistoryOfUser(username);
                    break;
                case '2':
                    PostMode();
                    break;
                case '3':
                    UserSearchMode();
                    break;
                case '4':
                    break;
                default:
                    std::cout << "Wrong input.\n";
                    break;
            }
        }  
    }
    std::cout << "Logout was successful.\n";
}

void Login()
{
    std::string enteredUsername = "";
    std::string enteredPassword = "";
    while (true)
    {
        std::cout << "Please enter your username.\n";
        std::cin >> enteredUsername;
        if (CheckUsernameForExistance(enteredUsername))
        {
            std::cout << "Please enter your password.\n";
            std::cin >> enteredPassword;
            if (CheckPassword(enteredPassword))
            {
                AccountPage(enteredUsername);
                break;
            }
        }
        std::cout << "This Username doesn't exist.\n";       
    }
}

void Register()
{
    std::string enteredUsername = "";
    std::string enteredPassword = "";
    while (true)
    {
        std::cout << "Please enter a username.\n";
        std::cin >> enteredUsername;
        if (CheckUsernameForExistance(enteredUsername)) // abfrage negieren nach erfolgreichem test
        {
            std::cout << "Your chosen username is " << enteredUsername << ".\n";
            std::cout << "Please enter a password.\n";
            std::cin >> enteredPassword;
            std::cout << "You registered succesfully.\n";
            // write to database
            AccountPage(enteredUsername);
            break;
        }
    }
}

void Start()
{
    std::string chosenOption = "";
    while (true)
    {
        std::cout << "Please enter 1 to login or 2 to register\n";
        std::cin >> chosenOption;
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

int main(int argc, char* argv[])
{
    WSAData d;
    if (WSAStartup(MAKEWORD(2, 2), &d))
    {
        printf("WinSocket failed to initialize\n");
        return -1;
    }
    if (argc < 3)
    {
        printf("usage tcp client [ip] [port]\n");
        return -1;
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
        return -1;
    } 
    printf("Remote adress is :\n");
    char adressBuffer[100];
    getnameinfo(server->ai_addr, server->ai_addrlen, adressBuffer, sizeof(adressBuffer), 0, 0, NI_NUMERICHOST);
    printf("%s\n", adressBuffer);
    printf("Creating socket...\n");
    SOCKET serverSocket = socket(server->ai_family, server->ai_socktype, server->ai_protocol);
    globalSocket = serverSocket;
    if (serverSocket == INVALID_SOCKET)
    {
        fprintf(stderr, "socket(9 failed. %d)\n", WSAGetLastError());
        return -1;
    }
    printf("Connection to server...\n");
    if (connect(serverSocket, server->ai_addr, server->ai_addrlen))
    {
        fprintf(stderr, "connect() failed. (%d)\n", WSAGetLastError());
        return -1;
    }
    freeaddrinfo(server);
    printf("Connected!\n");
    printf("To send data, enter the text followed by enter \n");

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
            return -1;
        }

        if (FD_ISSET(serverSocket, &reads))
        {
            char read[4096];
            int bytesReceived = recv(serverSocket, read, sizeof(read), 0);
            printf("Received size: %d\n", sizeof(read));
            printf("Received: %s", read);
            if (bytesReceived < 1)
            {
                printf("Connection closed.\n");
                return 1;
            }
        }       
        Start();

            //char read[4096];
            //if (!fgets(read, 4096, stdin))
            //{
            //    break;
            //}
            //printf("Sending %s", read);
            //int bytesSent = send(serverSocket, read, sizeof(read), 0);
            //printf("%d bytes sent\n", bytesSent);
    }
    WSACleanup();
    return 0;
}