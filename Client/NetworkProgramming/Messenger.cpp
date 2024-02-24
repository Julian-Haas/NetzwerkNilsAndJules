#pragma comment (lib, "ws2_32.lib")
#pragma comment (lib, "iphlpapi.lib")

#include <cstdio>
#include <conio.h>
#include <WinSock2.h>
#include <iphlpapi.h>
#include <WS2tcpip.h>
#include <string>

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

bool Messenger::CheckUsernameForExistance(std::string Username)
{
    char UsernameRequest[4096];
    UsernameRequest[0] = CheckUsernameForExistance_Client;
    int UsernameLength = Username.length();
    for (int i = 1; i < UsernameLength + 1; i++)
    {
        UsernameRequest[i] = Username[i - 1];
        //std::cout << RegisterRequest[i];
    }
    //std::cout << "\n";
    UsernameRequest[UsernameLength + 1] = '\0';
    send(serverSocket, UsernameRequest, 4096, 0);
    // wait for response and return bool
    //std::cout << "Error: Server is not implemented yet, returned a default value.\n";
    return true; // remove
}

bool Messenger::CheckPasswordForCorrectness(std::string Username, std::string Password)
{
    char PasswordRequest[4096];
    PasswordRequest[0] = CheckPasswordForCorrectness_Client;
    int UsernameLength = Username.length();
    for (int i = 1; i < UsernameLength + 1; i++)
    {
        PasswordRequest[i] = Username[i - 1];
        //std::cout << RegisterRequest[i];
    }
    //std::cout << "\n";
    PasswordRequest[UsernameLength + 1] = '\0';
    //std::cout << RegisterRequest << "\n";
    int PasswordLength = Password.length();
    for (int i = 101; i < PasswordLength + 101; i++)
    {
        PasswordRequest[i] = PasswordRequest[i - 101];
        //std::cout << RegisterRequest[i];
    }
    //std::cout << "\n";
    PasswordRequest[PasswordLength + 101] = '\0';
    //std::cout << RegisterRequest << "\n";
    bool LoginSuccessful = false;
    send(serverSocket, PasswordRequest, 4096, 0);
    // wait for response and return bool
    return true; //remove
}

void Messenger::DisplayHistoryOfUser(std::string username)
{
    //retrieve data and display it
    //send(serverSocket, constMessage, sizeof(constMessage), 0); // Username noch mitgeben
    std::cout << "This is a placepolder for the history of " << username << ".\n";
}

void Messenger::PostMode() // wie besser sockel mitgeben? klasse?
{
    std::string message; 
    while (true)
    {
        std::cout << "Please enter a message with maximum 280 characters which you want to post.\n";
        std::cin.ignore(10000, '\n');
        //std::cin.clear();
        //std::cin.sync();
        std::getline(std::cin, message);
        if (message.length() > 280)
        {
            std::cout << "Your message was too long.\n";
            continue;
        }
        const char* constMessage ='4' + message.c_str();
        send(serverSocket, constMessage, 4096, 0); // Username noch mitgeben
        //nachricht auf dem server speichern mit uhrzeit und username
        std::cout << "You posted this message: " << message << "\n";
        break;
    }
    while(true)
    {
        char anwser[4096]; 
        int anwserSize = 0; 
        anwserSize = recv(serverSocket, anwser, 4096, 0);
        if(anwserSize > 0)
        {
            printf(anwser);
            printf("\n"); 
            break; 
        }
    }
}

void Messenger::UserSearchMode()
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

void Messenger::AccountPage(std::string username)
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
                PostMode();
                correctInputFlag = true;
                break;
            case '3':
                UserSearchMode();
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
    char LoginRequest[4096];
    LoginRequest[0] = CheckPasswordForCorrectness_Client;
    //std::cout << "Protocol - Code: " << (int)RegisterRequest[0] << "\n";
    while (true)
    {
        while (true)
        {
            std::cout << "Please enter your username.\n";
            std::getline(std::cin, enteredUsername);
            if (!true) // true mit CheckUsernameForExistance() austauschen sobald server implementiert
            {
                std::cout << "This User doesn't exist.\n";
                continue;
            }
            break;
        }
        int UsernameLength = enteredUsername.length();
        //std::cout << "Username-Length: " << UsernameLength << "\n";
        for (int i = 1; i < UsernameLength + 1; i++)
        {
            LoginRequest[i] = enteredUsername[i - 1];
            //std::cout << RegisterRequest[i];
        }
        //std::cout << "\n";
        LoginRequest[UsernameLength + 1] = '\0';
        //std::cout << RegisterRequest << "\n";
        std::cout << "Your Username is " << enteredUsername << ".\n";
        std::cout << "Please enter your password.\n";
        std::getline(std::cin, enteredPassword);
        int PasswordLength = enteredPassword.length();
        for (int i = 101; i < PasswordLength + 101; i++)
        {
            LoginRequest[i] = enteredPassword[i - 101];
            //std::cout << RegisterRequest[i];
        }
        //std::cout << "\n";
        LoginRequest[PasswordLength + 101] = '\0';
        //std::cout << RegisterRequest << "\n";
        while (true)
        {
            //send(serverSocket, LoginRequest, 4096, 0);
            // wait for response : RegisterSuccessful = true;
            if (true) // austauschen mit RegisterSuccessful wenn server implemented
            {
                std::cout << "Your login was successful.\n";
                break;
            }
        }
        AccountPage(enteredUsername);
        break;
    }
}

void Messenger::Register()
{
    std::string enteredUsername;
    std::string enteredPassword;
    char RegisterRequest[4096];
    RegisterRequest[0] = RegisterUser_Client;
    //std::cout << "Protocol - Code: " << (int)RegisterRequest[0] << "\n";
    while (true)
    {
        while (true)
        {
            std::cout << "Please enter a username.\n";
            std::getline(std::cin, enteredUsername);
            if (false) // false mit CheckUsernameForExistance() austauschen sobald server implementiert
            {
                std::cout << "This Username is already in use.\n";
                continue;
            }
            break;
        }
        int UsernameLength = enteredUsername.length();
        //std::cout << "Username-Length: " << UsernameLength << "\n";
        for (int i = 1; i < UsernameLength + 1; i++)
        {
            RegisterRequest[i] = enteredUsername[i - 1];
            //std::cout << RegisterRequest[i];
        }
        //std::cout << "\n";
        RegisterRequest[UsernameLength + 1] = '\0';
        //std::cout << RegisterRequest << "\n";
        std::cout << "Your chosen username is " << enteredUsername << ".\n";
        std::cout << "Please enter a password.\n";
        std::getline(std::cin, enteredPassword);
        int PasswordLength = enteredPassword.length();
        for (int i = 101; i < PasswordLength + 101; i++)
        {
            RegisterRequest[i] = enteredPassword[i - 101];
            //std::cout << RegisterRequest[i];
        }
        //std::cout << "\n";
        RegisterRequest[PasswordLength + 101] = '\0';
        //std::cout << RegisterRequest << "\n";
        bool RegisterSuccessful = false;
        while (true)
        {
            //send(serverSocket, RegisterRequest, 4096, 0);
            // wait for response : RegisterSuccessful = true;
            if (true) // austauschen mit RegisterSuccessful wenn server implemented
            {
                std::cout << "You registered succesfully.\n";
                break;
            }
        }
        AccountPage(enteredUsername);
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
    if (argc < 3) // Lösungsvorschlag?
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
    if (getaddrinfo(argv[1], argv[2], &hints, &server)) // Lösungsvorschlag?
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

void Messenger::WaitForServer()
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
            return;
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
                return;
            }
        }
    }
}

void Messenger::SendToServer()
{

}

void Messenger::MainMenu()
{
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