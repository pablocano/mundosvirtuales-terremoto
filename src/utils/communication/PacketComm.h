#pragma once

#include "../security/Cryptography.h"

#include <string>
#include <cstring>
#include <cstdlib>
#include <memory>
#include <algorithm>

#define PORT_SERVERPLANT			4322 /* Define port for TCP communication.*/
#define MAYOR_NUM_VERSION_PACKET	0x1  /* Define mayor number of version. */
#define MENOR_NUM_VERSION_PACKET	0x0  /* Define menor number of version. */
#define VERSION_PACKET (((MAYOR_NUM_VERSION_PACKET) << 8) | (MENOR_NUM_VERSION_PACKET))  /* Define version of communication packet. */

#define MAX_ERROR_COMM 3 /* Define max number of error supported before it closed communication. */

#define COMM_ENCRYPTED /* If is defined, the packets are encrypted. */
#define KEY_CRYPT { 56394, 12134315, 74, 156 } /* Private key for encrypted. */

#ifdef WIN32
#define STRUCT_PACKET struct
#pragma pack(push)
#pragma pack(1)
#else
#define STRUCT_PACKET struct __attribute__((packed))
#endif

/// <summary>
/// Enum that contain different commands.
/// </summary>
enum Command
{
	ALIVE			   = 0x00, /* Alive command. */
	EARTHQUAKE		   = 0x01, /* This command is sent from the client when the sensors are activated due to an earthquake. */
	WRONG_PACKET	   = 0x02, /* This command is returned when the receive packet has wrong format. */
	CLOSE_CONNECTION   = 0x03, /* This command close connection between client and server. */
	// Adding new command from bellow ...
	ACKNOWLEDGE_CLIENT = 0x04, /* This command starts exchange data between clients and server. */
	RENEW_DEVICE_ID	   = 0x05, /* This command allows to renew device identification. */
};

/// <summary>
/// Header of Communication Packet.
/// </summary>
STRUCT_PACKET HeaderPacketComm
{
	uint16_t  version;		/* Version of packet. */
	uint8_t   m_command;	/* Command */
	uint8_t   m_flags;		/* Flags */
	uint32_t  m_idDevice;	/* Identification of sensor or client. */
	uint32_t  m_idResponse;	/* Identification for Response Packet. */

	/// <summary>
	/// Constructor
	/// </summary>
	HeaderPacketComm(const Command command = Command::ALIVE) : version(VERSION_PACKET), m_command(command), m_flags(0), m_idDevice(0), m_idResponse(0) {};

	/// <summary>
	/// Validate header.
	/// </summary>
	/// <returns>Returns true if header is valid, false otherwise.</returns>
	bool isValid() const
	{
		return version == VERSION_PACKET;
	}

	/// <summary>
	/// Operator equals.
	/// </summary>
	/// <param name="header">Header for comparing.</param>
	/// <returns>Returns true if all elements of header are equals, false otherwise.</returns>
	bool operator ==(const HeaderPacketComm &header) const
	{
		return (version == header.version) && (m_command == header.m_command) && (m_idResponse == header.m_idResponse);
	}
};

typedef float PAYLOAD;

/// <summary>
/// Packet for communication.
/// </summary>
STRUCT_PACKET PacketComm
{
	HeaderPacketComm m_header;  /* Header of Packet. */
	PAYLOAD			 m_payload;	/* Payload. */

	/// <summary>
	/// Constructor.
	/// </summary>
	PacketComm(const Command command = Command::ALIVE) : m_header(command) {}

	/// <summary>
	/// Validate packet.
	/// </summary>
	/// <returns>Returns true if packet has a valid format, false otherwise.</returns>
	bool isValid() const
	{
		return m_header.isValid();
	}

	/// <summary>
	/// Size of Packet.
	/// </summary>
	/// <returns>Size of Packet (header + payload).</returns>
	static int size()
	{
		return sizeHeader() + sizeContent();
	}

	/// <summary>
	/// Size of header.
	/// </summary>
	/// <returns>Size of header.</returns>
	static int sizeHeader()
	{
		return sizeof(HeaderPacketComm);
	}

	/// <summary>
	/// Size of content of packet.
	/// </summary>
	/// <returns>Size of payload.</returns>
	static int sizeContent()
	{
		return sizeof(PAYLOAD);
	}

	/// <summary>
	/// Gets index of current packet.
	/// </summary>
	/// <returns></returns>
	uint32_t getIndexRequest() const
	{
		static uint32_t index = 0;
		return ++index;
	}

	/// <summary>
	/// Packing Communication Packet.
	/// </summary>
	/// <returns>Returns.</returns>
	PacketComm packing() const
	{
		PacketComm p;
		p = *this;
		p.m_header.m_idResponse = m_header.m_idResponse ? m_header.m_idResponse : getIndexRequest();

#ifdef COMM_ENCRYPTED
		static uint32_t key[4] = KEY_CRYPT;
		Cryptography::encrypt((uint32_t*)&p, size() / 4, key);
#endif

		return p;
	}

	/// <summary>
	/// Unpacking Communication Packet.
	/// </summary>
	/// <param name="header">Header</param>
	/// <param name="header">Payload</param>
	/// <returns>New Communication Packet</returns>
	static PacketComm unpacking(HeaderPacketComm header, PAYLOAD payload)
	{
		PacketComm packet;
		packet.m_header = header;
		packet.m_payload = payload;

		return unpacking(packet);
	}

	/// <summary>
	/// Unpacking Communication Packet.
	/// </summary>
	/// <returns>New Communication Packet</returns>
	static PacketComm unpacking(PacketComm packet)
	{
#ifdef COMM_ENCRYPTED
		static uint32_t key[4] = KEY_CRYPT;
		Cryptography::decrypt((uint32_t*)&packet, packet.size() / 4, key);
#endif
		return packet;
	}

	/// <summary>
	/// Operator equals.
	/// </summary>
	/// <param name="packet">Packet for comparing.</param>
	/// <returns>Returns true if headers are equals, false otherwise.</returns>
	bool operator==(const PacketComm &packet) const
	{
		return m_header == packet.m_header;
	}
};

#ifdef WIN32
#pragma pack(pop)
#endif
