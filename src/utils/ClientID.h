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

/* Identification of remote connection. */
class ClientID
{
private:
	RemoteID m_id; /* Identification of remote client. */
	SessionID m_sid; /* Identification of session.  */

public:

	/// <summary>
	/// Constructor
	/// </summary>
	/// <param name="id"></param>
	ClientID(int id = 0) : m_id(id)
	{
		m_sid = generateSID();
	}

	/// <summary>
	/// Getter of identification remote client.
	/// </summary>
	/// <returns></returns>
	RemoteID getID() const
	{
		return m_id;
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
};