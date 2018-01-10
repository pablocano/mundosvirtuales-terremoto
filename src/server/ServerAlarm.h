#pragma once

#include "../server/ListSensors.h"
#include "../server/RequestAlarm.h"
#include "../server/ResponseAlarm.h"
#include "../utils/network/ServerTCP.h"

#define FILE_REMOTE_DATA_ALARMS "RemotesAlarmIDs.dat"
#define PORT_SERVER_ALARM 4323

struct PairClientData;

class ServerAlarm : public ServerTCP
{
private:

	RequestAlarm m_requestAlarm;
	ResponseAlarm m_responseAlarm;
	std::vector<PairClientData> m_listRemoteClientData;
	LocalID m_nextID;

	ServerAlarm(const ServerAlarm&) = delete;
	ServerAlarm & operator=(const ServerAlarm&) = delete;

	ServerAlarm();

	~ServerAlarm();

protected:

	RemoteID getNewRemoteID() const;

	LocalID getNextLocalID() const;

	virtual std::shared_ptr<ClientTCP> addClient(int socketClient) override;

	virtual void remove_client_from_queue(std::shared_ptr<ClientTCP> lpClient) override;

public:

	static ServerAlarm* Instance() {
		static ServerAlarm* S = new ServerAlarm();
		return S;
	}

	void stop() { m_requestAlarm.stop(); Runnable::stop(); }

	void start() { Runnable::start(); m_requestAlarm.start(); }

	friend class ServerAlarm;
};