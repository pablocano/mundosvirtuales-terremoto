#pragma once

#include "../server/ListSensors.h"
#include "../server/RequestAlarm.h"
#include "../server/ResponseAlarm.h"
#include "../utils/network/ServerTCP.h"

#define PORT_SERVER_ALARM 4323

class ServerAlarm : public ServerTCP
{
private:

	RequestAlarm m_requestAlarm;
	ResponseAlarm m_responseAlarm;

	ServerAlarm(const ServerAlarm&) = delete;
	ServerAlarm & operator=(const ServerAlarm&) = delete;

	ServerAlarm();

	~ServerAlarm();

public:

	static ServerAlarm* Instance() {
		static ServerAlarm* S = new ServerAlarm();
		return S;
	}

	void stop() { m_requestAlarm.stop(); Runnable::stop(); }

	void start() { Runnable::start(); m_requestAlarm.start(); }

	friend class ServerAlarm;
};