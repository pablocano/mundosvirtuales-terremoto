#include "ListSensors.h"

ListSensors::ListSensors() : m_nextPos(0), m_activeSensors(0), m_maxSize(0)
{
}

LocalID ListSensors::getNextLocalID()
{
	return m_nextPos;
}

void ListSensors::ListSensors::appendMeassure(LocalID id, MeassurementSensor& data)
{
	if (id >= 0 && id <= m_maxSize)
	{
		m_queue[id] = data;
	}
}

int ListSensors::getActiveSensors() const
{
	return m_activeSensors;
}

int ListSensors::getCurrentTotalSensors() const
{
	return m_maxSize;
}

bool ListSensors::reserveSpaceNewSensor()
{
	++m_maxSize;
	m_maxSize = (m_maxSize < MAX_SIZE_SENSORS ? m_maxSize : MAX_SIZE_SENSORS);

	++m_nextPos;
	if(m_nextPos < MAX_SIZE_SENSORS)
	{
		++m_activeSensors;
		return true;
	}
	else 
	{
		--m_nextPos;
		return false;
	}
}

void ListSensors::deleteSpaceSensor(LocalID id)
{
	--m_activeSensors;
	m_nextPos = id;
}

const MeassurementSensor& ListSensors::operator[](int index) const
{
	return m_queue[index];
}
