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
