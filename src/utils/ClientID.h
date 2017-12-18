#pragma once

#include <string>

typedef std::string UID;

typedef int ID;

class ClientID
{
private:
	ID m_id;
	UID m_uuid;

public:
	/// <summary>
	/// Constructor.
	/// </summary>
	ClientID() : m_id(0)
	{
		m_uuid = generateUUID();
	}

	ClientID(int id) : m_id(id)
	{
		m_uuid = generateUUID();
	}

	ID getID() const
	{
		return m_id;
	}

	const UID getUUID() const
	{
		return m_uuid;
	}

	static UID generateUUID()
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

		return (UID) uuid;
	}
};