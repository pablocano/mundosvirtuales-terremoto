#include "ResponseSensors.h"
#include "ServerSensors.h"

#include "../utils/logger/Logger.h"
#include "../utils/platform/SystemCall.h"


ResponseSensors::ResponseSensors()
{
}

ResponseSensors::~ResponseSensors()
{
}

PacketComm ResponseSensors::process_packet(const ClientID clientID, PacketComm packet, SocketClientTcp& tcpComm)
{
	PacketComm responsePacket;

	LOGGER_LOG("ResponseSensors", tcpComm.getInfo() + ":");
	switch (packet.m_header.m_command)
	{
	case Command::CLOSE_CONNECTION:
		LOGGER_LOG("ResponseSensors", "CLOSE_CONNECTION");
		tcpComm.closeSocket();
		break;
	case Command::EARTHQUAKE:
		LOGGER_LOG("ResponseSensors", ("EARTHQUAKE : " + std::to_string(packet.m_payload)));
		updateListSensor(clientID, packet, SystemCall::getCurrentSystemTime());
		break;
	case Command::ALIVE:
		LOGGER_LOG("ResponseSensors", "ALIVE");
		break;
	case Command::NONE:
		LOGGER_LOG("ResponseSensors", "NONE");
		break;
	default:
		LOGGER_LOG("ResponseSensors", "UNKNOW COMMAND");
	}

	return responsePacket;
}

void ResponseSensors::updateListSensor(const ClientID clientID, PacketComm packet, long timestamp)
{
	DataSensor data;
	data.m_timestamp = timestamp;
	data.m_intensity = packet.m_payload;

	ServerSensors::Instance()->m_listSensors.appendMeassure(clientID.getID(), data);
}
