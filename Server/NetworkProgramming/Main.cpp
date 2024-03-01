#include "Server.h"
int main(int argc, char* argv[])
{
	std::vector<std::vector<std::string>> baseArr(10, std::vector<std::string>(2));
	Server server(baseArr); 
	server.InitServer(argc, argv);
	return 0; 
}