#pragma once

#include "../utils/communication/ResponsePacketServer.h"

#include <map>

/// <summary>
/// Process incoming packets, if it is necessary, this class makes a query to database for response some packets.
/// </summary>
class ServerSensors : public ResponsePacketServer
{
public:
	/// <summary>
	/// Constructor.
	/// </summary>
	ServerSensors();

	/// <summary>
	/// Destructor.
	/// </summary>
	~ServerSensors();

	/// <summary>
	/// Automatically respond incoming packets, if a packet respond to a request packet, the packet is placed in a packet queue.
	/// </summary>
	/// <param name="packet">Incoming packet.</param>
	/// <param name="tcpComm">Socket communication.</param>
	/// <returns>Returns response packet for the incoming packet.</returns>
	virtual PacketComm process_packet(PacketComm packet, SocketClientTcp& tcpComm) override;
};