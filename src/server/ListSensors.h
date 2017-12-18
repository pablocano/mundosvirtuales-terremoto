#pragma once

#include "../utils/ClientID.h"

#define MAX_SIZE_SENSORS 10000

struct DataSensor
{
	float m_intensity;
	long m_timestamp;
	float m_confidence;

	DataSensor() : m_intensity(0.f), m_timestamp(0L), m_confidence(1.f) {};
};

/// <summary>
/// List of sensors with concurrency support.
/// </summary>
class ListSensors
{
protected:
	DataSensor m_queue[MAX_SIZE_SENSORS]; /* Queue where the data is stored. */
	ID m_lastID;

public:

	/// <summary>
	/// Constructor
	/// </summary>
	ListSensors();

	/// <summary>
	/// Reserve space for a new sensor and returns its ID. 
	/// </summary>
	/// <returns></returns>
	ID appendNewIDSensor();

	/// <summary>
	/// Added an item to queue.
	/// </summary>
	/// <param name="id">Identification client sensor.</param>
	/// <param name="data">New sensor to add to queue.</param>
	void appendMeassure(ID id, DataSensor& data);

	int size() const
	{
		return m_lastID;
	}

	const DataSensor& operator[](int index) const
	{
		return m_queue[index];
	}
};
