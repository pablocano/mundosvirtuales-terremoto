#pragma once

#include "TcpComm.h"
#include "ClientTCP.h"
#include "../Queue.h"
#include "../communication/ResponsePacketServer.h"
#include "../concurrency/Runnable.h"

/// <summary>
/// Server TCP handles tcp communication for each remote client.
/// </summary>
class ServerTCP : public Runnable
{
protected:
	SocketServerTCP* m_lpTcpServer; /* Socket server. */
	ResponsePacketServer* m_lpServerResponse; /* Pointer to class responding to messages (packet).  */
	Queue<std::shared_ptr<ClientTCP>> m_queueClients; /* Queue of active clients (where each client is attended with a thread). */
	int m_port; /* Port TCP. */

	void run(); /* Implement virtual function. */

	/// <summary>
	/// Remove client from queue.
	/// </summary>
	/// <param name="lpClient">Pointer to handle communication with client.</param>
	virtual void remove_client_from_queue(std::shared_ptr<ClientTCP> lpClient);

public:
	/// <summary>
	/// Constructor.
	/// </summary>
	ServerTCP(ResponsePacketServer* lpServer = nullptr, int port = PORT_SERVERPLANT);

	/// <summary>
	/// Destructor.
	/// </summary>
	~ServerTCP();

	/// <summary>
	/// Stops Server (close all clients).
	/// </summary>
	void stop();

	/// <summary>
	/// Determine if exist clients.
	/// </summary>
	/// <returns>Returns true if there are active clients, false otherwise.</returns>
	bool exist_client();

	/// <summary>
	/// Validate if the remote client already stored in list of sensors.
	/// </summary>
	/// <param name="clientID"></param>
	/// <returns>Returns true if the client exist in the list of sensors, false otherwise.</returns>
	bool existRemoteClient(const ClientID& clientID) const;

	/// <summary>
	/// Gets the local identification from remote identification.
	/// </summary>
	/// <param name="remoteID">Remote identification.</param>
	/// <returns>Returns a local identification.</returns>
	LocalID getLocalIDFromRemoteID(const RemoteID& remoteID) const;

	/// <summary>
	/// Close all active clients.
	/// </summary>
	void close_all_clients();

	/// <summary>
	/// Getting queue of clients.
	/// </summary>
	/// <returns></returns>
	const Queue<std::shared_ptr<ClientTCP>>& getQueueClients() const;

protected:

	/// <summary>
	/// Adding clients for communication.
	/// </summary>
	/// <param name="socketClient">Socket of client</param>
	/// <returns></returns>
	virtual std::shared_ptr<ClientTCP> addClient(int socketClient);
};
