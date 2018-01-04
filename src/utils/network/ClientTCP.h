#pragma once

#include "TcpComm.h"
#include "../Queue.h"
#include "../concurrency/Runnable.h"
#include "../communication/ResponsePacket.h"
#include "../ClientID.h"

/// <summary>
/// Client TCP handles tcp communication with a thread.
/// </summary>
class ClientTCP : public Runnable, public std::enable_shared_from_this<ClientTCP>
{
protected:
	SocketClientTcp m_tcpComm; /* Socket TCP. */
	std::function< void ( std::shared_ptr<ClientTCP> ) > m_closeClientFn; /* Callback function when the communication finishes. */
	ResponsePacket* m_lpResponsePacket; /* Pointer to class responding to messages (packet). */
	ClientID m_clientID; /* Identification of client. */

	void run(); /* Implement virtual function. */

public:
	/// <summary>
	/// Constructor.
	/// IMPORTANT:
	///		By default the thread NOT detach of parent thread.
	/// </summary>
	/// <param name="ip">IP or DNS of remote host.</param>
	/// <param name="port">Port of remote host.</param>
	/// <param name="m_lpResponsePacket">Pointer to class responding to messages (packet).</param>
	/// <param name="closeClientFn">Callback function when the communication finishes.</param>
	/// <param name="isDetach">Whether this flag is true the thread detaches of parent thread.</param>
	ClientTCP(const char* ip, int port, ResponsePacket* m_lpResponsePacket, std::function<void(std::shared_ptr<ClientTCP>)> closeClientFn = nullptr, bool isDetach = false);

	/// <summary>
	/// Constructor.
	/// IMPORTANT:
	///		By default the thread detach of parent thread.
	/// </summary>
	/// <param name="socket">Handle socket.</param>
	/// <param name="m_lpResponsePacket">Pointer to class responding to messages (packet).</param>
	/// <param name="closeClientFn">Callback function when the communication finishes.</param>
	/// <param name="isDetach">Whether this flag is true the thread detaches of parent thread.</param>
	ClientTCP(int socket, ResponsePacket* m_lpResponsePacket, std::function<void(std::shared_ptr<ClientTCP>)> closeClientFn = nullptr, bool isDetach = true);

	/// <summary>
	/// Destructor.
	/// </summary>
	~ClientTCP();

	/// <summary>
	/// Send messages.
	/// </summary>
	/// <param name="message">Buffer message.</param>
	/// <param name="size">Size buffer message.</param>
	/// <returns>Returns true if the message was sent successful, false otherwise.</returns>
	bool sendMessage(const char* message, unsigned int size);

	/// <summary>
	/// Receive message.
	/// </summary>
	/// <param name="message">Buffer message.</param>
	/// <param name="size">Size buffer message.</param>
	/// <returns>Returns true if the message was received successful, false otherwise.</returns>
	bool recvMessage(char* message, unsigned int size);

	/// <summary>
	/// Setter of identification of remote client.
	/// </summary>
	/// <param name="clientID">Identification of remote client.</param>
	void setClientID(ClientID clientID);

	/// <summary>
	/// Getter Identification of client.
	/// </summary>
	const ClientID getClientID() const { return m_clientID; }
};
