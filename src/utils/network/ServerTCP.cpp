#include "ServerTCP.h"
#include "ServerTCP.h"
#include "ServerTCP.h"
#include "ExceptionNetwork.h"
#include "../logger/Logger.h"
#include "../platform/SystemCall.h"

ServerTCP::ServerTCP(ResponsePacketServer* lpServer, int port) : m_lpServerResponse(lpServer), m_port(port)
{
	LOGGER_LOG("Server TCP", "Create Server");
}

ServerTCP::~ServerTCP()
{
	m_lpTcpServer->closeSocket();

	close_all_clients();

	delete m_lpTcpServer;
	try { stop(); }
	catch (...) { /*??*/ }

	LOGGER_LOG("Server TCP", "Delete Thread Server");
}

void ServerTCP::run()
{
	if (m_lpServerResponse)
	{
		m_lpTcpServer = new SocketServerTCP(m_port);
		m_lpTcpServer->setBlock(true);
		// m_lpTcpServer->setTimeout(10000); // TODO: doesn't work :(

		LOGGER_LOG("Server TCP", "Run Server");
		while (!m_stop || exist_client())
		{
			int socketClient = -1;
			if ((socketClient = m_lpTcpServer->acceptClient()) > 0)
			{
				std::shared_ptr<ClientTCP> lpClient = addClient(socketClient);

				if (lpClient)
				{
					lpClient->start();
				} else {
					LOGGER_LOG("Server TCP", "An error occurred while trying to add a new client.");
				}
			}
		}
		LOGGER_LOG("Server TCP", "Stop Server");
	}
	else
	{
		LOGGER_LOG("Server TCP", "Invalid Server");
	}
}

std::shared_ptr<ClientTCP> ServerTCP::addClient(int socketClient)
{
	std::shared_ptr<ClientTCP> lpClient(new ClientTCP(socketClient,
		(ResponsePacket *)m_lpServerResponse, std::bind(&ServerTCP::remove_client_from_queue, this, std::placeholders::_1)));

	m_queueClients.add(lpClient);

	return lpClient;
}

void ServerTCP::stop()
{
	close_all_clients();

	Runnable::stop();
}

bool ServerTCP::exist_client()
{
	return m_queueClients.size() > 0;
}

bool ServerTCP::existRemoteClient(const ClientID & clientID) const
{
	for (int i = 0; i < m_queueClients.size(); ++i)
	{
		if (m_queueClients.at(i)->getClientID().getRemoteID() == clientID.getRemoteID())
		{
			return true;
		}
	}
	return false;
}

LocalID ServerTCP::getLocalIDFromRemoteID(const RemoteID & remoteID) const
{
	for (int i = 0; i < m_queueClients.size(); ++i)
	{
		const ClientID& clientID = m_queueClients.at(i)->getClientID();
		if (m_queueClients.at(i)->getClientID().getRemoteID() == remoteID)
		{
			return clientID.getLocalID();
		}
	}
	return LocalID(-1);
}

void ServerTCP::close_all_clients()
{
	while (exist_client())
	{
		std::shared_ptr<ClientTCP> lpClient = m_queueClients.remove();
		lpClient->stop();
	}
}
const Queue<std::shared_ptr<ClientTCP>>& ServerTCP::getQueueClients() const
{
	return m_queueClients;
}

void ServerTCP::remove_client_from_queue(std::shared_ptr<ClientTCP> lpClient)
{
	m_queueClients.remove(lpClient);
}