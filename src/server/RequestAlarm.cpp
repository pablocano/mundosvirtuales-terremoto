#include "RequestAlarm.h"
#include "RequestAlarm.h"
#include "ServerSensors.h"
#include "../utils/platform/SystemCall.h"
#include "../utils/logger/Logger.h"

#include <string>

RequestAlarm::RequestAlarm(const Queue<std::shared_ptr<ClientTCP>>& queueClients) : m_queueClients(queueClients)
{
}

void RequestAlarm::run()
{
	LOGGER_DEBUG("RequestAlarm", "Start Request Alarm");
	const ListSensors& listSensors = ServerSensors::Instance()->getListSensors();
	while (!m_stop)
	{
		SystemCall::sleep(500);
		long timestamp = SystemCall::getCurrentSystemTime();
		float intensity = 0.f;
		int validSensors = 0;

		int totalSensors = listSensors.getCurrentTotalSensors();
		if (totalSensors > 0)
		{
			for (int i = 0; i < totalSensors; ++i)
			{
				if ((timestamp - listSensors[i].m_timestamp) < 2000)
				{
					if (listSensors[i].m_confidence > 0.5f)
					{
						validSensors++;
						intensity += listSensors[i].m_intensity;
					}
				}
			}

			float meanIntensity = intensity / validSensors;

			if (listSensors.getActiveSensors() / totalSensors > 0.5f)
			{
				sendAlert(meanIntensity);
			}
		}
	}
	
	LOGGER_DEBUG("RequestAlarm", "Stop Request Alarm");
}

void RequestAlarm::sendAlert(float meanIntensity) const
{
	PacketComm packetAlert;

	packetAlert.m_header.m_command = Command::EARTHQUAKE;
	packetAlert.m_header.m_idDevice = 0;
	packetAlert.m_header.m_idResponse = 0;
	packetAlert.m_payload = meanIntensity;

	int size = m_queueClients.size();
	LOGGER_DEBUG("RequestAlarm", "Send alarm EarthQuake " + std::to_string(packetAlert.m_payload));
	for (int i = 0; i < size; ++i)
	{
		PacketComm packet = packetAlert.packing();
		m_queueClients.at(i).get()->sendMessage((char *) &packet, sizeof(PacketComm));
	}
}
