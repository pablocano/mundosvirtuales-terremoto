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
		ALIVE			   = 0x00, /* Alive command. */
		EARTHQUAKE		   = 0x01, /* This command is sent from the client when the sensors are activated due to an earthquake. */
		WRONG_PACKET	   = 0x02, /* This command is returned when the receive packet has wrong format. */
		CLOSE_CONNECTION   = 0x03, /* This command close connection between client and server. */
		// Adding new command from bellow ...
		ACKNOWLEDGE_SENSOR = 0x04, /* This command starts exchange data between clients and server. */
		RENEW_DEVICE_ID	   = 0x05  /* This command allows to renew device identification. */
	};

	struct MessageHeader
	{
		bool isValid();
		short version;
		byte type;
		byte message;
		unsigned long id;
		unsigned long messageId;
		float data;
	};

	///<summary>
	/// Constructor of the class
	///</summary>
	/// <param name="_id">The unique id of this device.</param>
	ServerComm(unsigned long _id) : id(_id), messageId(0L){}
	
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
	bool StartComm(char* server, int port);
	
	///<summary>
	/// Send a message to the server
	///</summary>
	/// <param name="type">The type of message to send.</param>
	/// <param name="data">Custom data to send.</param>
	/// <param name="size">The size of the custom data.</param>
	/// <returns>If the message was send</returns>
	bool SendMessage(MessageType type, float* ratio = 0);

	///<summary>
	/// Receive a message from the server
	///</summary>
	/// <param name="inMessage">The message received.</param>
	/// <returns>If a message was receive</returns>
	bool ReceiveMessage(MessageHeader& inMessage);
	
	
	bool Connected();
	
	void StopClient();
	
private:
	
	///<summary>
	/// An Ethernet client used to communicate information to the server
	///</summary>
	EthernetClient client;
	
	///<summary>
	/// The unique id of this device
	///</summary>
	unsigned long id;

	///<summary>
	/// The unique id of the next message
	///</summary>
	unsigned long messageId;
	
	///<summary>
	/// The version of the header
	///</summary>
	static const short version;
	
	///<summary>
	/// The key used to encryp and decrypt a message
	///</summary>
	static const unsigned long key[4];
};

#endif // __SERVERCOMM__
