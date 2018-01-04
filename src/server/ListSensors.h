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
	MeassurementSensor m_queue[MAX_SIZE_SENSORS]; /* Queue where the measurements are stored. */
	LocalID m_nextPos; /* Position where the next measurement will be stored. */
	int m_activeSensors; /* Amount of active sensors. */
	int m_maxSize; /* Maximum amount of sensors. */

public:

	/// <summary>
	/// Constructor
	/// </summary>
	ListSensors();

	/// <summary>
	/// Returns index for current space for saving meassurent. 
	/// </summary>
	/// <returns></returns>
	LocalID getNextLocalID();

	/// <summary>
	/// Adding an item to queue.
	/// </summary>
	/// <param name="id">Identification client sensor.</param>
	/// <param name="data">New sensor to add to queue.</param>
	void appendMeassure(LocalID id, MeassurementSensor& data);

	/// <summary>
	/// Getting active sensors. 
	/// </summary>
	/// <returns></returns>
	int getActiveSensors() const;

	/// <summary>
	/// Getting amount of sensors (active and inactive sensors).
	/// </summary>
	/// <returns></returns>
	int getCurrentTotalSensors() const;

	/// <summary>
	/// Reserving space for new sensor meassurement.
	/// </summary>
	/// <returns>Returns true if the reservation was successful, false otherwise.</returns>
	bool reserveSpaceNewSensor();

	/// <summary>
	/// Deleting space of sensor.
	/// </summary>
	/// <param name="id">local identification of sensor.</param>
	void deleteSpaceSensor(LocalID id);

	/// <summary>
	/// Operator indexation.
	/// </summary>
	/// <param name="index">Index</param>
	/// <returns>Returns meassurement stored in position</returns>
	const MeassurementSensor& operator[](int index) const;
};
