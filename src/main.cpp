
#include <iostream>
#include <string>
#include "utils/network/ServerTCP.h"
#include "server/ServerSensors.h"
#include "utils/platform/SystemCall.h"


int main() {
	
	ServerTCP server(new ServerSensors());

	server.start();

	SystemCall::sleep(60000);

	server.stop();

	std::cout << "Press key to exit" << std::endl;
	std::getchar();

	return 0;
}
	