#include "ServerSensors.h"
#include "../utils/logger/Logger.h"


ServerSensors::ServerSensors()
{
}

ServerSensors::~ServerSensors()
{
}

PacketComm ServerSensors::process_packet(PacketComm packet, SocketClientTcp& tcpComm)
{
	PacketComm responsePacket;

	LOGGER_LOG("ResponsePacketServer", tcpComm.getInfo() + ":");
	switch (packet.m_header.m_command)
	{
	case Command::CLOSE_CONNECTION:
		LOGGER_LOG("ResponsePacketServer", "CLOSE_CONNECTION");
		tcpComm.closeSocket();
		break;
	case Command::EARTHQUAKE:
		LOGGER_LOG("ResponsePacketServer", ("EARTHQUAKE : " + std::to_string(packet.m_payload)));
		break;
	case Command::ALIVE:
		LOGGER_LOG("ResponsePacketServer", "ALIVE");
		break;
	case Command::NONE:
		LOGGER_LOG("ResponsePacketServer", "NONE");
		break;
	default:
		LOGGER_LOG("ResponsePacketServer", "UNKNOW COMMAND");
	}

	return responsePacket;
}