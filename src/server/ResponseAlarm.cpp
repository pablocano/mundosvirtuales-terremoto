#include "ResponseAlarm.h"
#include "../server/ListSensors.h"
#include "../utils/logger/Logger.h"
#include "../utils/platform/SystemCall.h"


ResponseAlarm::ResponseAlarm(ListSensors& listSensors) : m_listSensors(listSensors)
{
}

ResponseAlarm::~ResponseAlarm()
{
}

PacketComm ResponseAlarm::process_packet(const ClientID clientID, PacketComm packet, SocketClientTcp& tcpComm)
{
	PacketComm responsePacket;
	std::string p = "Alarm" + clientID.toString() + ": ";

	switch (packet.m_header.m_command)
	{
	case Command::CLOSE_CONNECTION:
		LOGGER_LOG("ResponseAlarm", p + "CLOSE_CONNECTION");
		tcpComm.closeSocket();
		break;
	case Command::ALIVE:
		LOGGER_LOG("ResponseAlarm", p + "ALIVE");
		break;
	default:
		LOGGER_LOG("ResponseAlarm", p + "UNKNOW COMMAND");
	}

	return responsePacket;
}
