#include "ServerSensors.h"
#include "../utils/logger/Logger.h"
#include "../utils/platform/SystemCall.h"

#include <fstream>
#include <string>

ServerSensors::ServerSensors() : m_listSensors(), m_listRemoteClientData(),  m_serverSensors(), ServerTCP(&m_serverSensors)
{
	std::fstream fileRemoteIDs;
	fileRemoteIDs.open(FILE_REMOTE_DATA_SENSORS, std::ios::in | std::ios::binary);

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

ServerSensors::~ServerSensors()
{
}

const ListSensors& ServerSensors::getListSensors() const
{
	return m_listSensors;
}

RemoteID ServerSensors::getNewRemoteID() const
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

std::shared_ptr<ClientTCP> ServerSensors::addClient(int socketClient)
{
	std::shared_ptr<ClientTCP> lpClient = ServerTCP::addClient(socketClient);

	if (lpClient) // Validating client pointer.
	{
		// Getting the next ID in list of sensors.
		LocalID newID = ServerSensors::Instance()->m_listSensors.getNextLocalID();

		if (newID >= 0) // Validating ID
		{
			bool validRemoteID = false;
			bool isNewRemoteClient = false;
			int posLastClient = -1;
			RemoteID remoteID = -1;

			// Send Acknowledge command
			LOGGER_DEBUG("ServerSensor", "Send Acknowledge");
			PacketComm packetAck(Command::ACKNOWLEDGE_CLIENT);
			lpClient->sendMessage(packetAck);

			// Receive Response
			PacketComm packetRecv;
			if (lpClient->recvMessage(packetRecv))
			{
				LOGGER_DEBUG("ServerSensor", "Recv Response Acknowledge");
				if (packetRecv.m_header.m_command == Command::ACKNOWLEDGE_CLIENT)
				{
					remoteID = (RemoteID) packetRecv.m_header.m_idDevice;
					LOGGER_DEBUG("ServerSensor", "Recv remote id: " + std::to_string(remoteID));
					if (remoteID < 0) /* Validate remote identification. */
					{
						PacketComm packetRenew(Command::RENEW_DEVICE_ID);

						// Generate new identification.
						remoteID = getNewRemoteID();

						// Store new identification in payload of packet
						packetRenew.m_payload = (float) remoteID; // TODO: copy memory
						
						// Send packet with new identification.
						LOGGER_DEBUG("ServerSensor", "Send renew message");
						if (lpClient->sendMessage(packetRenew))
						{
							// Receive Response
							if (lpClient->recvMessage(packetRecv) && packetRecv.m_header.m_command == Command::ACKNOWLEDGE_CLIENT)
							{
								LOGGER_DEBUG("ServerSensor", "Recv correct response renew, ID: " + std::to_string(packetRecv.m_header.m_idDevice));
								// Validate if the new remote identification was changed successful.  
								if (packetRecv.m_header.m_idDevice == remoteID)
								{
									isNewRemoteClient = true;
									validRemoteID = true;

									LOGGER_LOG("ServerSensor", "New sensor was added, Remote ID: " + std::to_string(remoteID));
								}
							}
							else
							{
								LOGGER_DEBUG("ServerSensor", "Problem Recv Response renew");
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
									LOGGER_LOG("ServerSensor", "Remote ID already exist.");
								}
								else
								{
									// Reserve space for
									ServerSensors::Instance()->m_listSensors.reserveSpaceNewSensor();
								}

								m_listRemoteClientData[i].m_lpClientTCP = lpClient;

								LOGGER_LOG("ServerSensor", "Sensor ID: " + std::to_string(remoteID) + " was added successful (LOCAL ID " + std::to_string(newID) + ")");
								lpClient->setClientID(ClientID(newID, remoteID));

								return lpClient;
							}
						}
					}
				}
			}
			else
			{
				LOGGER_LOG("ServerSensor", "Error Receive Message Acknowledge");
			}

			// space in list of sensor is reserved if the remote identification is valid.   
			if (validRemoteID && ServerSensors::Instance()->m_listSensors.reserveSpaceNewSensor())
			{
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

				LOGGER_LOG("ServerSensor", "Sensor ID: " + std::to_string(remoteID) + " was added successful (LOCAL ID " + std::to_string(newID) + ")");

				// Return new client
				return lpClient;
			}

		}

		// Close thread and socket.
		lpClient->stop();
		LOGGER_LOG("ServerSensor", "Problem added new remote sensor");
	}

	return nullptr;
}

void ServerSensors::remove_client_from_queue(std::shared_ptr<ClientTCP> lpClient)
{
	// Remove client from queue.
	ServerTCP::remove_client_from_queue(lpClient);

	if (!lpClient->getWasReplaced())
	{
		// Free up space if the client was not replaced
		LocalID localID = lpClient->getClientID().getLocalID();
		ServerSensors::Instance()->m_listSensors.deleteSpaceSensor(localID);
	}
}
