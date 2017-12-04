#include "ClientTCP.h"
#include "../logger/Logger.h"

#include <iostream>

ClientTCP::ClientTCP(const char* ip, int port, std::shared_ptr<ResponsePacket> lpProcessPacket, std::function<void(std::shared_ptr<ClientTCP>)> closeClientFn, bool isDetach) : Runnable(isDetach),
m_tcpComm(ip, port), m_lpResponsePacket(lpProcessPacket), m_closeClientFn(closeClientFn)
{
}

ClientTCP::ClientTCP(int socket, std::shared_ptr<ResponsePacket> lpProcessPacket, std::function<void(std::shared_ptr<ClientTCP>)> closeClientFn, bool isDetach) : Runnable(isDetach),
m_tcpComm(socket), m_lpResponsePacket(lpProcessPacket), m_closeClientFn(closeClientFn)
{
}

ClientTCP::~ClientTCP()
{
	m_tcpComm.closeSocket();
	LOGGER_DEBUG("Client TCP", "Delete Thread Communication Client " + m_tcpComm.getInfo());
}

void ClientTCP::run()
{
	int count_error = 0;
	LOGGER_DEBUG("Client TCP", "Run Client " + m_tcpComm.getInfo());
	char* lpBuffer = nullptr;
	while (!m_stop)
	{
		PacketComm packet;

		try
		{
			// Read header packet.
			if (m_tcpComm.receive((char *)&packet, PacketComm::size()))
			{

#ifdef COMM_ENCRYPTED
				PacketComm packetReceive = PacketComm::unpacking(packet);
#else
				PacketComm packetReceive = packet;
#endif
				
				// Validate packet.
				if (packetReceive.isValid())
				{
					// Process packet, returns a response if it necessary.
					PacketComm packetResponse = m_lpResponsePacket->process_packet(packetReceive, m_tcpComm);
							
					if (packetResponse.isValid())
					{
						// Response packaging.
						PacketComm packetTCP = packetResponse.packing();
						// Send response.
						m_tcpComm.send((char *) &packetTCP, packetTCP.size());
					}
				}
				else
				{
					// Return wrong version response.
					LOGGER_ERROR("Client TCP", "Error Version Packet");
					count_error++;

					PacketComm packet;
					packet.m_header.m_command = Command::WRONG_PACKET;
					packet.m_payload = 0.f;

					PacketComm packetTCP = packet.packing();
					m_tcpComm.send((char *) &packetTCP, packet.size());
				}
			}
			else
			{
				count_error++;
			}
		}
		catch (const std::exception& e)
		{
			LOGGER_ERROR("ClientTCP", e.what());
			count_error++;
		}

		if (count_error >= MAX_ERROR_COMM)
			m_stop = true;
	}

	LOGGER_DEBUG("Client TCP", "Stop Communication Client " + m_tcpComm.getInfo());
	if (lpBuffer)
		std::free(lpBuffer); // Free buffer memory.

	// This function is called function when the server was closed.
	if (m_closeClientFn)
		m_closeClientFn(shared_from_this());
}