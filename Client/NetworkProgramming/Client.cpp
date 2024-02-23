#include "Messenger.h"

int main(int argc, char* argv[])
{
    Messenger messenger;
    messenger.StartMessenger(argc, argv);
    WSACleanup();
    return 0;
}

//mutmaßlich Müll, später löschen

//char read[4096];
//if (!fgets(read, 4096, stdin))
//{
//    break;
//}
//printf("Sending %s", read);
//int bytesSent = send(serverSocket, read, sizeof(read), 0);
//printf("%d bytes sent\n", bytesSent);

//int messageLength = message.length();
//std::cout << "messageLength: " << messageLength << "\n";
//std::cout << "message: " << message << "\n";
//int messageLength = strlen(message)-1;
//char* test = new char[messageLength];
//const char* constMessage = message.c_str();
//std::cout << messageLength << "\n";
//char read[4096] = {'s', 's', 's', 's', 's', 's'};
//char read2[4096] = { '1', '2', '3'};
//std::cout << read2 << "\n";
//std::cout << sizeof(read2) << "\n";
//printf("Sending %s", read);
//int bytesSent = send(serverSocket, read2, sizeof(read), 0);
//printf("%d bytes sent\n", bytesSent);
//std::cout << message << "\n";
//message = "idsugfsuzdfg dsifusghdfiusgd";
//std::cout << constMessage << "\n";
//const char* constMessage = "siugsdhsiudfz fizsdfisdfuz";
//string to const char*
//string to char array
//char read[4096] = message.c_str();
//char read[4096] = "siugsdhsiudfz fizsdfisdfuz";

/*    while (true)
    {
        std::cout << "Please enter your username.\n";
        std::cin.ignore(10000, '\n');
        std::getline(std::cin, enteredUsername);
        if (CheckUsernameForExistance(enteredUsername))
        {
            std::cout << "Please enter your password.\n";
            std::cin >> enteredPassword;
            if (CheckPasswordForCorrectness(enteredUsername, enteredPassword))
            {
                AccountPage(enteredUsername);
                break;
            }
        }
        std::cout << "This Username doesn't exist.\n";    */