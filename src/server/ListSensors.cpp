#include "ListSensors.h"


ListSensors::ListSensors() : m_lastID(0)
{
}

ID ListSensors::appendNewIDSensor()
{
	ID id = m_lastID++;
	return id;
}

void ListSensors::ListSensors::appendMeassure(ID id, DataSensor& data)
{
	if (id >= 0 && id <= m_lastID)
	{
		m_queue[id] = data;
	}
}
