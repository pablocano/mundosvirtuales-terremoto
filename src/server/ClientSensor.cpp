#include "ClientSensor.h"
#include "../utils/logger/Logger.h"

#include <iostream>

ClientSensor::ClientSensor()
{
	LOGGER_LOG("Response Packet Client", "Create client");
}

ClientSensor::~ClientSensor()
{
	LOGGER_LOG("Response Packet Client", "Delete obj client");
}

PacketComm ClientSensor::process_packet(PacketComm packet, SocketClientTcp& tcpComm)
{
	PacketComm responsePacket;

	LOGGER_LOG("Response Packet Client", tcpComm.getInfo() + ":");
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
		m_queueResponsePacket.add(packet);
	}

	return responsePacket;
}