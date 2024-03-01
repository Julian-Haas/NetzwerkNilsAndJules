#include "Messenger.h"

int main(int argc, char* argv[])
{
    Messenger messenger;
    messenger.StartMessenger(argc, argv);
    WSACleanup();
    return 0;
}