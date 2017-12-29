#include "ListSensors.h"


ListSensors::ListSensors() : m_lastID(0)
{
}

RemoteID ListSensors::appendNewIDSensor()
{
	RemoteID id = m_lastID++;
	return id;
}

void ListSensors::ListSensors::appendMeassure(RemoteID id, MeassurementSensor& data)
{
	if (id >= 0 && id <= m_lastID)
	{
		m_queue[id] = data;
	}
}
