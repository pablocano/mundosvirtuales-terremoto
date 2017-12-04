#include "ServerTCP.h"
#include "ExceptionNetwork.h"
#include "../logger/Logger.h"

ServerTCP::ServerTCP(ResponsePacketServer* lpServer) : m_lpServerPlant(lpServer)
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
	if (m_lpServerPlant)
	{
		m_lpTcpServer = new SocketServerTCP(PORT_SERVERPLANT);
		LOGGER_LOG("Server TCP", "Run Server");
		while (!m_stop || exist_client())
		{
			int socketClient = 0;
			if ((socketClient = m_lpTcpServer->acceptClient()) > 0)
			{
				std::shared_ptr<ClientTCP> lpClient(new ClientTCP(socketClient,
					std::dynamic_pointer_cast<ResponsePacket>(m_lpServerPlant), std::bind(&ServerTCP::remove_client_from_queue, this, std::placeholders::_1)));

				m_queueClients.add(lpClient);

				lpClient->start();
			}
		}
		LOGGER_LOG("Server TCP", "Stop Server");
	}
	else
	{
		LOGGER_LOG("Server TCP", "Invalid Server");
	}
}

bool ServerTCP::exist_client()
{
	return m_queueClients.size() > 0;
}

void ServerTCP::close_all_clients()
{
	while (exist_client())
	{
		std::shared_ptr<ClientTCP> lpClient = m_queueClients.remove();
		lpClient->stop();
	}
}

void ServerTCP::remove_client_from_queue(std::shared_ptr<ClientTCP> lpClient)
{
	m_queueClients.remove(lpClient);
}