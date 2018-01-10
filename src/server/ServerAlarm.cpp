#include "ServerAlarm.h"
#include "ServerSensors.h"
#include "../utils/logger/Logger.h"
#include "../utils/platform/SystemCall.h"

#include <fstream>

ServerAlarm::ServerAlarm() : m_responseAlarm(ServerSensors::Instance()->m_listSensors), ServerTCP::ServerTCP(&m_responseAlarm, PORT_SERVER_ALARM), m_requestAlarm(getQueueClients()), m_listRemoteClientData(), m_nextID(0)
{
	std::fstream fileRemoteIDs;
	fileRemoteIDs.open(FILE_REMOTE_DATA_ALARMS, std::ios::in | std::ios::binary);

	// Loading registered clients from file.
	if (fileRemoteIDs.good())
	{
		fileRemoteIDs.seekg(0, std::ios::beg);

		while (!fileRemoteIDs.eof())
		{
			PairClientData dataReg;
			fileRemoteIDs.read((char*)&dataReg.m_data, sizeof(RemoteClientData));
			// Loading one register
			m_listRemoteClientData.push_back(dataReg);
		}
	}

	fileRemoteIDs.close();
}

ServerAlarm::~ServerAlarm()
{
}

RemoteID ServerAlarm::getNewRemoteID() const
{
	// A valid remote id is always positive.
	RemoteID id = 1;

	bool validID = true;

	// Validate if the list of remote client is empty
	if (!m_listRemoteClientData.empty())
	{
		// looking for a new valid identification 
		while (validID)
		{
			validID = false;
			++id;
			for (auto it = m_listRemoteClientData.begin(); it != m_listRemoteClientData.end(); ++it)
			{
				if (it->m_data.m_remoteID == id)
				{
					validID = true;
					break;
				}
			}
		}
	}

	return id;
}

LocalID ServerAlarm::getNextLocalID() const
{
	return m_nextID;
}

std::shared_ptr<ClientTCP> ServerAlarm::addClient(int socketClient)
{
	std::shared_ptr<ClientTCP> lpClient = ServerTCP::addClient(socketClient);

	if (lpClient) // Validating client pointer.
	{
		// Getting the next ID in list of sensors.
		LocalID newID = getNextLocalID();

		if (newID >= 0) // Validating ID
		{
			bool validRemoteID = false;
			bool isNewRemoteClient = false;
			int posLastClient = -1;
			RemoteID remoteID = -1;

			// Send Acknowledge command
			LOGGER_DEBUG("ServerAlarm", "Send Acknowledge");
			PacketComm packetAck(Command::ACKNOWLEDGE_CLIENT);
			lpClient->sendMessage(packetAck);

			// Receive Response
			PacketComm packetRecv;
			if (lpClient->recvMessage(packetRecv))
			{
				LOGGER_DEBUG("ServerAlarm", "Recv Response Acknowledge");
				if (packetRecv.m_header.m_command == Command::ACKNOWLEDGE_CLIENT)
				{
					remoteID = (RemoteID)packetRecv.m_header.m_idDevice;
					LOGGER_DEBUG("ServerAlarm", "Recv remote id: " + std::to_string(remoteID));
					if (remoteID < 0) /* Validate remote identification. */
					{
						PacketComm packetRenew(Command::RENEW_DEVICE_ID);

						// Generate new identification.
						remoteID = getNewRemoteID();

						// Store new identification in payload of packet
						packetRenew.m_payload = (float)remoteID; // TODO: copy memory

						// Send packet with new identification.
						LOGGER_DEBUG("ServerAlarm", "Send renew message");
						if (lpClient->sendMessage(packetRenew))
						{
							// Receive Response
							if (lpClient->recvMessage(packetRecv) && packetRecv.m_header.m_command == Command::ACKNOWLEDGE_CLIENT)
							{
								LOGGER_DEBUG("ServerAlarm", "Recv correct response renew, ID: " + std::to_string(packetRecv.m_header.m_idDevice));
								// Validate if the new remote identification was changed successful.  
								if (packetRecv.m_header.m_idDevice == remoteID)
								{
									isNewRemoteClient = true;
									validRemoteID = true;

									LOGGER_LOG("ServerAlarm", "New alarm was added, Remote ID: " + std::to_string(remoteID));
								}
							}
							else
							{
								LOGGER_DEBUG("ServerAlarm", "Problem Recv Response renew");
							}
						}
					}
					else
					{
						validRemoteID = true;
					}

					// Validate if the remoteid is already in server.
					if (validRemoteID)
					{
						for (int i = 0; i < m_listRemoteClientData.size(); ++i)
						{
							if (remoteID == m_listRemoteClientData.at(i).m_data.m_remoteID)
							{
								std::shared_ptr<ClientTCP> lastClient = m_listRemoteClientData[i].m_lpClientTCP;
								if (lastClient)
								{
									// Close last client
									lastClient->stop();
									newID = lastClient->getClientID().getLocalID();
									lastClient->setwasReplace(true);
									LOGGER_LOG("ServerAlarm", "Remote ID already exist.");
								}
								else
								{
									m_nextID++;
								}

								m_listRemoteClientData[i].m_lpClientTCP = lpClient;

								LOGGER_LOG("ServerAlarm", "Sensor ID: " + std::to_string(remoteID) + " was added successful (LOCAL ID " + std::to_string(newID) + ")");
								lpClient->setClientID(ClientID(newID, remoteID));

								return lpClient;
							}
						}
					}
				}
			}
			else
			{
				LOGGER_LOG("ServerAlarm", "Error Receive Message Acknowledge");
			}

			if (validRemoteID)
			{
				// space in list of sensor is reserved if the remote identification is valid.   
				m_nextID++;

				// Setting identification in new client 
				lpClient->setClientID(ClientID(newID, remoteID));

				// Create register
				PairClientData dataReg;
				dataReg.m_data.m_createTimestamp = SystemCall::getCurrentSystemTime();
				dataReg.m_data.m_lat = -33.4381664;
				dataReg.m_data.m_lng = -70.6474686;
				dataReg.m_data.m_remoteID = (RemoteID)packetRecv.m_header.m_idDevice;
				dataReg.m_lpClientTCP = lpClient;

				if (isNewRemoteClient)
				{
					// Save register in file.
					std::fstream fileRemoteIDs;
					fileRemoteIDs.open(FILE_REMOTE_DATA_SENSORS, std::ios::binary | std::ios::app);
					fileRemoteIDs.write((const char*)&dataReg.m_data, sizeof(RemoteClientData));
					fileRemoteIDs.close();

					// Save register in list
					m_listRemoteClientData.push_back(dataReg);
				}

				LOGGER_LOG("ServerAlarm", "Alarm ID: " + std::to_string(remoteID) + " was added successful (LOCAL ID " + std::to_string(newID) + ")");

				// Return new client
				return lpClient;
			}

		}

		// Close thread and socket.
		lpClient->stop();
		LOGGER_LOG("ServerAlarm", "Problem added new remote alarm");
	}

	return nullptr;
}

void ServerAlarm::remove_client_from_queue(std::shared_ptr<ClientTCP> lpClient)
{
	// Remove client from queue.
	ServerTCP::remove_client_from_queue(lpClient);

	if (!lpClient->getWasReplaced())
	{
		// Free up space if the client was not replaced
		m_nextID = lpClient->getClientID().getLocalID();
	}
}
