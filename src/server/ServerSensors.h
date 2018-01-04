#pragma once

#include "../server/ListSensors.h"
#include "../server/ResponseSensors.h"
#include "../utils/network/ServerTCP.h"

#include <vector>

#define FILE_REMOTE_DATA_CLIENTS "RemotesIDs.dat"

class ServerAlarm;

class ServerSensors : public ServerTCP
{
private:

	ListSensors m_listSensors;
	std::vector<RemoteClientData> m_listRemoteClientData;
	ResponseSensors m_serverSensors;

	ServerSensors(const ServerSensors&) = delete;
	ServerSensors & operator=(const ServerSensors&) = delete;

	ServerSensors();

	~ServerSensors();

public:

	static ServerSensors* Instance() {
		static ServerSensors* S = new ServerSensors();
		return S;
	}

	const ListSensors& getListSensors() const;

	RemoteID getNewRemoteID() const;

	friend class ServerAlarm;
	friend class ResponseSensors;

protected:

	virtual std::shared_ptr<ClientTCP> addClient(int socketClient) override;

	virtual void remove_client_from_queue(std::shared_ptr<ClientTCP> lpClient) override;
};