#pragma once

#include "../utils/ClientID.h"

#define MAX_SIZE_SENSORS 10000

/// <summary>
/// Information of sensor
/// </summary>
struct MeassurementSensor
{
	float m_intensity; /* Intensity of earthquake. */
	long m_timestamp; /* Timestamp of meassurement.  */
	float m_confidence; /* Confidence of meassurement. */

	MeassurementSensor() : m_intensity(0.f), m_timestamp(0L), m_confidence(1.f) {};
};

/// <summary>
/// List of sensors without concurrency support.
/// </summary>
class ListSensors
{
protected:
	MeassurementSensor m_queue[MAX_SIZE_SENSORS]; /* Queue where the data is stored. */
	RemoteID m_lastID; /*  */

public:

	/// <summary>
	/// Constructor
	/// </summary>
	ListSensors();

	/// <summary>
	/// Reserve space for a new sensor and returns its ID. 
	/// </summary>
	/// <returns></returns>
	RemoteID appendNewIDSensor();

	/// <summary>
	/// Added an item to queue.
	/// </summary>
	/// <param name="id">Identification client sensor.</param>
	/// <param name="data">New sensor to add to queue.</param>
	void appendMeassure(RemoteID id, MeassurementSensor& data);

	int size() const
	{
		return m_lastID;
	}

	const MeassurementSensor& operator[](int index) const
	{
		return m_queue[index];
	}
};
