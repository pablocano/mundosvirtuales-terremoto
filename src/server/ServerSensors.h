#pragma once

#include "../server/ListSensors.h"
#include "../server/ResponseSensors.h"
#include "../utils/network/ServerTCP.h"

#include <vector>
#include <memory>

#define FILE_REMOTE_DATA_SENSORS "RemotesIDs.dat"

class ServerAlarm;

struct PairClientData
{
	RemoteClientData m_data;
	std::shared_ptr<ClientTCP> m_lpClientTCP;

	PairClientData() : m_data(), m_lpClientTCP(nullptr) {}
};

class ServerSensors : public ServerTCP
{
private:

	ListSensors m_listSensors;
	std::vector<PairClientData> m_listRemoteClientData;
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