#include "ServerSensors.h"


ServerSensors::ServerSensors() : m_listSensors(), m_serverSensors(), ServerTCP(&m_serverSensors)
{
}

ServerSensors::~ServerSensors()
{
}

const ListSensors & ServerSensors::getListSensors() const
{
	return m_listSensors;
}

std::shared_ptr<ClientTCP> ServerSensors::addClient(int socketClient)
{
	std::shared_ptr<ClientTCP> lpClient = ServerTCP::addClient(socketClient);

	RemoteID newID = ServerSensors::Instance()->m_listSensors.appendNewIDSensor();

	lpClient->setClientID(ClientID(newID));

	return lpClient;
}
