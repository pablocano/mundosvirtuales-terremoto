#pragma once

#include "../utils/communication/ResponsePacket.h"

/// <summary>
/// Process incoming packets for client.
/// </summary>
class ResponsePacketClient : public ResponsePacket
{
public:
	/// <summary>
	/// Constructor.
	/// </summary>
	ResponsePacketClient() {}

	/// <summary>
	/// Destructor.
	/// </summary>
	~ResponsePacketClient() {}

	/// <summary>
	/// Automatically respond incoming packets, if a packet respond to a request packet, the packet is placed in a packet queue.
	/// </summary>
	/// <param name="packet">Incoming packet.</param>
	/// <param name="tcpComm">Socket communication.</param>
	/// <returns>Returns response packet for the incoming packet.</returns>
	virtual PacketComm process_packet(PacketComm packet, SocketClientTcp& tcpComm) = 0;
};
