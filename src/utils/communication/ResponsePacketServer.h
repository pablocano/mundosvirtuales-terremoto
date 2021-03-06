#pragma once

#include "ResponsePacket.h"
#include "../network/ClientTCP.h"

/// <summary>
/// Process incoming packets, if it is necessary, this class makes a query to database for response some packets.
/// </summary>
class ResponsePacketServer : public ResponsePacket
{
public:
	/// <summary>
	/// Constructor.
	/// </summary>
	ResponsePacketServer() {}

	/// <summary>
	/// Destructor.
	/// </summary>
	~ResponsePacketServer() {}

	/// <summary>
	/// Automatically respond incoming packets, if a packet respond to a request packet, the packet is placed in a packet queue.
	/// </summary>
	/// <param name="clientID">Identification of client.</param>
	/// <param name="packet">Incoming packet.</param>
	/// <param name="tcpComm">Socket communication.</param>
	/// <returns>Returns response packet for the incoming packet.</returns>
	virtual PacketComm process_packet(const ClientID clientID, PacketComm packet, SocketClientTcp& tcpComm) = 0;
};