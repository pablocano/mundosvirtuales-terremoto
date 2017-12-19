#pragma once

#include "../server/ListSensors.h"
#include "../server/ResponseSensors.h"
#include "../utils/network/ServerTCP.h"

class ServerAlarm;

class ServerSensors : public ServerTCP
{
private:

	ListSensors m_listSensors;
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

	friend class ServerAlarm;
	friend class ResponseSensors;

protected:

	virtual std::shared_ptr<ClientTCP> addClient(int socketClient) override;
};