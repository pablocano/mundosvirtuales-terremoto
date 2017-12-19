#pragma once


#pragma once

#include "../utils/concurrency/Runnable.h"
#include "../utils/network/ClientTCP.h"
#include "../utils/Queue.h"

class RequestAlarm : public Runnable
{
private:
	const Queue<std::shared_ptr<ClientTCP>>& m_queueClients; /* Reference to queue of active clients. */

public:

	RequestAlarm(const Queue<std::shared_ptr<ClientTCP>>& listClients);

	void run(); /* Implement virtual function. */

	void sendAlert(float meanIntensity) const;
};