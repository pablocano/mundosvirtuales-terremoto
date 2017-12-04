#include "ResponsePacket.h"
#include "ResponsePacket.h"

PacketComm ResponsePacket::get_response(PacketComm &request)
{
	std::shared_ptr<PacketComm> response = m_queueResponsePacket.remove(request); // pop response from queue

	if (response)
	{
		return *response;
	}
	else
	{
		return PacketComm();
	}
}

bool ResponsePacket::sendResponse(SocketClientTcp& tcpComm, PacketComm& packet, PAYLOAD& payload)
{
	HeaderPacketComm header;
	PacketComm responsePacket;
	responsePacket.m_header.m_command = packet.m_header.m_command;
	responsePacket.m_header.m_idResponse = packet.m_header.m_idResponse;
	responsePacket.m_payload = payload;

	PacketComm packetTCP = responsePacket.packing();

	return tcpComm.send((char *) &packetTCP, packetTCP.size());
}