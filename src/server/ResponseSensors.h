#pragma once

#include "../utils/communication/ResponsePacketServer.h"
#include "../utils/network/ClientTCP.h"
#include "../utils/ClientID.h"
#include "../server/ListSensors.h"

/// <summary>
/// Process incoming packets, if it is necessary, this class makes a query to database for response some packets.
/// </summary>
class ResponseSensors : public ResponsePacketServer
{
public:
	/// <summary>
	/// Constructor.
	/// </summary>
	ResponseSensors();

	/// <summary>
	/// Destructor.
	/// </summary>
	~ResponseSensors();

	/// <summary>
	/// Automatically respond incoming packets, if a packet respond to a request packet, the packet is placed in a packet queue.
	/// </summary>
	/// <param name="clientID">Identification of client.</param>
	/// <param name="packet">Incoming packet.</param>
	/// <param name="tcpComm">Socket communication.</param>
	/// <returns>Returns response packet for the incoming packet.</returns>
	virtual PacketComm process_packet(const ClientID clientID, PacketComm packet, SocketClientTcp& tcpComm) override;

	/// <summary>
	/// Update list of sensor.
	/// </summary>
	/// <param name="clientID">Identification of client.</param>
	/// <param name="packet">Incoming packet.</param>
	/// <param name="timestamp">Timestamp when packet arrives to server.</param>
	void updateListSensor(const ClientID clientID, PacketComm packet, long timestamp);
};
