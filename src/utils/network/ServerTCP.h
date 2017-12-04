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
	std::shared_ptr<ResponsePacketServer> m_lpServerPlant; /* Pointer to class responding to messages (packet).  */
	Queue<std::shared_ptr<ClientTCP>> m_queueClients; /* Queue of active clients (where each client is attended with a thread). */

	void run(); /* Implement virtual function. */

	/// <summary>
	/// Remove client from queue.
	/// </summary>
	/// <param name="lpClient">Pointer to handle communication with client.</param>
	void remove_client_from_queue(std::shared_ptr<ClientTCP> lpClient);

public:
	/// <summary>
	/// Constructor.
	/// </summary>
	ServerTCP(ResponsePacketServer* lpServer = nullptr);

	/// <summary>
	/// Destructor.
	/// </summary>
	~ServerTCP();

	/// <summary>
	/// Determine if exist clients.
	/// </summary>
	/// <returns>Returns true if there are active clients, false otherwise.</returns>
	bool exist_client();

	/// <summary>
	/// Close all active clients.
	/// </summary>
	void close_all_clients();
};
