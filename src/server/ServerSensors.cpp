#include "ServerSensors.h"
#include "../utils/platform/SystemCall.h"

#include <fstream>

ServerSensors::ServerSensors() : m_listSensors(), m_listRemoteClientData(),  m_serverSensors(), ServerTCP(&m_serverSensors)
{
	std::fstream fileRemoteIDs;
	fileRemoteIDs.open(FILE_REMOTE_DATA_CLIENTS, std::ios::in | std::ios::binary);

	while (fileRemoteIDs.eof())
	{
		RemoteClientData dataReg;
		fileRemoteIDs.read((char*)&dataReg, sizeof(RemoteClientData));
		m_listRemoteClientData.push_back(dataReg);
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
	RemoteID id = m_listRemoteClientData.end()->m_remoteID;
	
	bool validID;

	do
	{
		validID = true;
		++id;
		for (auto it = m_listRemoteClientData.begin(); it != m_listRemoteClientData.end(); ++it)
		{
			if (it->m_remoteID == id)
			{
				validID = false;
				break;
			}
		}
	} while (validID);

	return id;
}

std::shared_ptr<ClientTCP> ServerSensors::addClient(int socketClient)
{
	std::shared_ptr<ClientTCP> lpClient = ServerTCP::addClient(socketClient);

	if (lpClient) // Validating client pointer.
	{
		// Getting the next ID in list of sensors.
		LocalID newID = ServerSensors::Instance()->m_listSensors.getNextLocalID();

		if (newID > 0) // Validating ID
		{
			bool validRemoteID = false;

			// Send Acknowledge command
			PacketComm packetAck(Command::ACKNOWLEDGE_SENSOR);
			lpClient->sendMessage((const char*)&packetAck, sizeof(PacketComm));

			// Receive Response
			PacketComm packetRecv;
			if (lpClient->recvMessage((char*)&packetRecv, sizeof(PacketComm)))
			{
				if (packetRecv.m_header.m_command == Command::ACKNOWLEDGE_SENSOR)
				{
					if (packetRecv.m_header.m_idDevice < 0) /* Validate remote identification. */
					{
						PacketComm packetRenew(Command::RENEW_DEVICE_ID);

						// Generate new identification.
						packetRenew.m_payload = (float) getNewRemoteID();
						
						// Send packet with new identification.
						if (lpClient->sendMessage((const char*)&packetRenew, sizeof(PacketComm)))
						{
							// Receive Response
							if (lpClient->recvMessage((char*)&packetRecv, sizeof(PacketComm)))
							{
								// Validate if the new remote identification was changed successful.  
								if (packetRecv.m_header.m_idDevice > 0)
								{
									validRemoteID = true;
									
									// Create register
									RemoteClientData dataReg;
									dataReg.m_createTimestamp = SystemCall::getCurrentSystemTime();
									dataReg.m_lat = -33.4381664;
									dataReg.m_lng = -70.6474686;
									dataReg.m_remoteID = (RemoteID) packetRecv.m_header.m_idDevice;

									// Save register in file.
									std::fstream fileRemoteIDs;
									fileRemoteIDs.open(FILE_REMOTE_DATA_CLIENTS, std::ios::binary | std::ios::app);
									fileRemoteIDs.write((const char*)&dataReg, sizeof(RemoteClientData));
									fileRemoteIDs.close();

									// Save register in list
									m_listRemoteClientData.push_back(dataReg);
								}
							}
						}
					}
					else
					{
						validRemoteID = true;
					}
				}
			}

			if (validRemoteID && ServerSensors::Instance()->m_listSensors.reserveSpaceNewSensor())
			{
				lpClient->setClientID(ClientID(newID));

				return lpClient;
			}

		}

		// Close thread and socket.
		lpClient->stop();
	}

	return nullptr;
}

void ServerSensors::remove_client_from_queue(std::shared_ptr<ClientTCP> lpClient)
{
	ServerTCP::remove_client_from_queue(lpClient);

	LocalID localID = lpClient->getClientID().getLocalID();

	ServerSensors::Instance()->m_listSensors.deleteSpaceSensor(localID);
}
