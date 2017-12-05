#ifndef __SERVERCOMM__
#define __SERVERCOMM__

#include "Arduino.h"
#include <Ethernet.h>

#define MAYOR_NUM_VERSION_PACKET	0x1  /* Define the mayor number of version. */
#define MINOR_NUM_VERSION_PACKET	0x0  /* Define the minor number of version. */

/// <summary>
/// Helper class to encryp and decryp content
/// </summary>
class ServerComm
{
public:

	///<summary>
	/// The different types of messages
	///</summary>
	enum MessageType
	{
		EARTHQUAKE,
		ALIVE
	};

	///<summary>
	/// Constructor of the class
	///</summary>
	/// <param name="_id">The unique id of this device.</param>
	ServerComm(unsigned long _id) : id(_id) {}
	
	///<summary>
	/// Init the ethernet communications
	///</summary>
	/// <param name="_macAddress">The mac address of the ethernet device.</param>
	bool Begin(byte* macAddress);
	
	///<summary>
	/// Start the communications with the server
	///</summary>
	/// <param name="_serverIp">The ip of the server.</param>
	/// <param name="_port">The port used to communicate information to the server.</param>
	/// <returns>If the connection was establish</returns>
	bool StartComm(IPAddress server, int port);
	
	///<summary>
	/// Send a message to the server
	///</summary>
	/// <param name="type">The type of message to send.</param>
	/// <param name="data">Custom data to send.</param>
	/// <param name="size">The size of the custom data.</param>
	bool SendMessage(MessageType type, float* ratio = 0);
	
	///<summary>
	/// An Ethernet client used to communicate information to the server
	///</summary>
	EthernetClient client;
	
	///<summary>
	/// The unique id of this device
	///</summary>
	unsigned long id;
	
	///<summary>
	/// The key used to encryp and decrypt a message
	///</summary>
	static const unsigned long key[4];
};

#endif // __SERVERCOMM__