#include "ServerAlarm.h"
#include "ServerSensors.h"


ServerAlarm::ServerAlarm() : m_responseAlarm(ServerSensors::Instance()->m_listSensors), ServerTCP::ServerTCP(&m_responseAlarm, PORT_SERVER_ALARM), m_requestAlarm(getQueueClients())
{
}

ServerAlarm::~ServerAlarm()
{
}
