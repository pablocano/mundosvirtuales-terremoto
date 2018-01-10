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
	std::string p = "Sensor" + clientID.toString() + ": ";

	switch (packet.m_header.m_command)
	{
	case Command::CLOSE_CONNECTION:
		LOGGER_LOG("ResponseSensors", p + "CLOSE_CONNECTION");
		// Close socket
		tcpComm.closeSocket();
		break;
	case Command::EARTHQUAKE:
		LOGGER_LOG("ResponseSensors", p + "EARTHQUAKE : " + std::to_string(packet.m_payload));
		// Adding new meassurement
		updateListSensor(clientID, packet, SystemCall::getCurrentSystemTime());
		break;
	case Command::ALIVE:
		LOGGER_LOG("ResponseSensors", p + "ALIVE");
		break;
	case Command::ACKNOWLEDGE_CLIENT:
		LOGGER_LOG("ResponseSensors", p + "ACKNOWLEDGE");
		{
			// Close connection if exist remote client on list of sensors.
			LocalID localID = ServerSensors::Instance()->getLocalIDFromRemoteID(clientID.getRemoteID());
			if (localID >= 0)
			{
				// Close connection.
				tcpComm.closeSocket();
			}
		}
		break;
	default:
		LOGGER_LOG("ResponseSensors", p + "UNKNOW COMMAND");
	}

	return responsePacket;
}

void ResponseSensors::updateListSensor(const ClientID clientID, PacketComm packet, long timestamp)
{
	MeassurementSensor data;
	data.m_timestamp = timestamp;
	data.m_intensity = packet.m_payload;

	ServerSensors::Instance()->m_listSensors.appendMeassure(clientID.getLocalID(), data);
}
