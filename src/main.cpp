
#include "utils/network/ServerTCP.h"
#include "server/ResponseSensors.h"
#include "server/RequestAlarm.h"
#include "utils/platform/SystemCall.h"
#include "server/ServerSensors.h"
#include "server/ServerAlarm.h"

#include <iostream>
#include <string>


int main() {

	ServerSensors* serverSensors = ServerSensors::Instance();
	ServerAlarm* serverAlarm = ServerAlarm::Instance();

	serverAlarm->start();
	serverSensors->start();

	char c = 0;
	while (c != 'Q' && c != 'q')
	{
		c = std::getchar();
	}

	serverSensors->stop();
	serverAlarm->stop();

	std::cout << "Press key to exit" << std::endl;
	std::cin.ignore();
	std::getchar();

	return 0;
}
	