#pragma once

#include <string>

/// <summary>
/// Session identification.
/// </summary>
typedef std::string SessionID;

/// <summary>
/// Identification of remote client.
/// </summary>
typedef int RemoteID;

/// <summary>
/// Register for storing data of remote client.
/// </summary>
struct RemoteClientData
{
	RemoteID m_remoteID; /* Remote identification */
	long m_createTimestamp; /* Timestamp when this register was stored. */
	double m_lat; /* latitude. */
	double m_lng; /* Longitude. */
};

/// <summary>
/// Local identification for client.
/// </summary>
typedef int LocalID;

/* Identification of remote connection. */
class ClientID
{
private:
	LocalID m_localID; /* Local identification for client. */
	RemoteID m_remoteID; /* Identification of remote client. */
	SessionID m_sid; /* Identification of session.  */

public:

	/// <summary>
	/// Constructor
	/// </summary>
	/// <param name="id">Local Identification</param>
	ClientID(LocalID id = -1, RemoteID rid = -1) : m_localID(id), m_remoteID(rid)
	{
		m_sid = generateSID();
	}

	/// <summary>
	/// Getter of identification remote client.
	/// </summary>
	/// <returns></returns>
	RemoteID getRemoteID() const
	{
		return m_remoteID;
	}

	/// <summary>
	/// Getter of local identification for client.
	/// </summary>
	/// <returns></returns>
	RemoteID getLocalID() const
	{
		return m_localID;
	}

	/// <summary>
	/// Getter Session of this remote connection.
	/// </summary>
	/// <returns></returns>
	const SessionID getSID() const
	{
		return m_sid;
	}

	/// <summary>
	/// Generate a random session identification.
	/// </summary>
	/// <returns></returns>
	static SessionID generateSID()
	{
		static const std::string CHARS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

		std::string uuid = std::string(36, ' ');
		int rnd = 0;
		int r = 0;

		uuid[8] = '-';
		uuid[13] = '-';
		uuid[18] = '-';
		uuid[23] = '-';

		uuid[14] = '4';

		for (int i = 0; i<36; i++) 
		{
			if (i != 8 && i != 13 && i != 18 && i != 14 && i != 23) 
			{
				if (rnd <= 0x02) 
					rnd = 0x2000000 + (std::rand() * 0x1000000) | 0;

				rnd >>= 4;
				uuid[i] = CHARS[(i == 19) ? ((rnd & 0xf) & 0x3) | 0x8 : rnd & 0xf];
			}
		}

		return (SessionID) uuid;
	}

	std::string toString() const
	{
		return "Sensor<" + std::to_string(m_remoteID) + ">";
	}
};