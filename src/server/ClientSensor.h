#pragma once

#include "../utils/communication/ResponsePacketClient.h"

/// <summary>
/// Process incoming packets for client.
/// </summary>
class ClientSensor : public ResponsePacketClient
{
public:
	/// <summary>
	/// Constructor.
	/// </summary>
	ClientSensor();

	/// <summary>
	/// Destructor.
	/// </summary>
	~ClientSensor();

	/// <summary>
	/// Automatically respond incoming packets, if a packet respond to a request packet, the packet is placed in a packet queue.
	/// </summary>
	/// <param name="packet">Incoming packet.</param>
	/// <param name="tcpComm">Socket communication.</param>
	/// <returns>Returns response packet for the incoming packet.</returns>
	virtual PacketComm process_packet(PacketComm packet, SocketClientTcp& tcpComm);
};
