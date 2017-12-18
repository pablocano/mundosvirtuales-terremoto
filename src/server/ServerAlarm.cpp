#include "ServerAlarm.h"
#include "ServerSensors.h"


ServerAlarm::ServerAlarm() : m_responseAlarm(ServerSensors::Instance()->m_listSensors), ServerTCP::ServerTCP(&m_responseAlarm, PORT_SERVER_ALARM), m_requestAlarm(getQueueClients())
{
}

ServerAlarm::~ServerAlarm()
{
}

std::shared_ptr<ClientTCP> ServerAlarm::addClient(int socketClient)
{
	std::shared_ptr<ClientTCP> lpClient = ServerTCP::addClient(socketClient);

	ID newID = ServerSensors::Instance()->m_listSensors.appendNewIDSensor();

	lpClient->setClientID(ClientID(newID));

	return lpClient;
}
