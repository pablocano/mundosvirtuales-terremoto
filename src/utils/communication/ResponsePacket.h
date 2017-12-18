#pragma once

#include "PacketComm.h"
#include "../Queue.h"
#include "../network/TcpComm.h"
#include "../ClientID.h"

/// <summary>
/// Abstract class allows to response packet.
/// </summary>
class ResponsePacket
{
protected:
	Queue<PacketComm> m_queueResponsePacket; /* Queue of incoming packets. */

public:
	/// <summary>
	/// Constructor.
	/// </summary>
	ResponsePacket() : m_queueResponsePacket() {};

	/* Deleting copy constructor */
	ResponsePacket(ResponsePacket const&) = delete;
	ResponsePacket& operator =(ResponsePacket const&) = delete;

	/// <summary>
	/// Automatically respond incoming packets, if a packet respond to a request packet, the packet is placed in a packet queue.
	/// </summary>
	/// <param name="clientID">Identification of client.</param>
	/// <param name="packet">Incoming packet.</param>
	/// <param name="tcpComm">Socket communication.</param>
	/// <returns>Returns response packet for the incoming packet.</returns>
	virtual PacketComm process_packet(const ClientID clientID, PacketComm packet, SocketClientTcp& tcpComm) = 0;

	/// <summary>
	/// Get response packet for specific request packet. This funciton looks for response in incoming packet queue.
	/// </summary>
	/// <param name="request">Request packet.</param>
	/// <returns>Returns response packet if this exist in queue, otherwise a new packet.</returns>
	PacketComm get_response(PacketComm &request);

	/// <summary>
	/// Sends response packet.
	/// </summary>
	/// <param name="tcpComm">Socket TCP.</param>
	/// <param name="packet">Packet that is necessary to answer.</param>
	/// <param name="lpContent">Packet Content to send as response.</param>
	/// <returns></returns>
	bool sendResponse(SocketClientTcp& tcpComm, PacketComm& packet, PAYLOAD& payload);
};