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
	ResponsePacket* m_lpResponsePacket; /* Pointer to class responding to messages (packet). */
	std::function< void ( std::shared_ptr<ClientTCP> ) > m_closeClientFn; /* Callback function when the communication finishes. */
	ClientID m_clientID; /* Identification of client. */
	bool m_wasReplaced; /* This flag is true when this client is replaced for another client. */

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
	/// Send packet.
	/// </summary>
	/// <param name="packet">Packet.</param>
	/// <returns>Returns true if the packet was sent successful, false otherwise.</returns>
	bool sendMessage(const PacketComm& packet);

	/// <summary>
	/// Receive message.
	/// </summary>
	/// <param name="message">Buffer message.</param>
	/// <param name="size">Size buffer message.</param>
	/// <returns>Returns true if the message was received successful, false otherwise.</returns>
	bool recvMessage(char* message, unsigned int size);

	/// <summary>
	/// Receive packet.
	/// </summary>
	/// <param name="packet">packet.</param>
	/// <returns>Returns true if the message was received successful, false otherwise.</returns>
	bool recvMessage(PacketComm &packet);

	/// <summary>
	/// Setter of identification of remote client.
	/// </summary>
	/// <param name="clientID">Identification of remote client.</param>
	void setClientID(ClientID clientID);

	/// <summary>
	/// Getter Identification of client.
	/// </summary>
	const ClientID getClientID() const { return m_clientID; }

	/// <summary>
	/// Returns true if this client was replaced for another.
	/// </summary>
	/// <returns></returns>
	bool getWasReplaced() const;

	/// <summary>
	/// Sets true when this client is replaced for another.
	/// </summary>
	/// <param name="wasReplaced"></param>
	void setwasReplace(bool wasReplaced);

protected:

	/// <summary>
	/// Close communication.
	/// </summary>
	void closeComm();

	virtual void preStop() override; /* Closing communications. */
};
