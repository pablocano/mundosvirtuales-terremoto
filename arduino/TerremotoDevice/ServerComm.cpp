#include "ServerComm.h"
#include "Cryptography.h"

const unsigned long ServerComm::key[4] = { 56394, 12134315, 74, 156 };

bool ServerComm::Begin(byte* macAddress)
{
	// start the Ethernet connection:
	int result = Ethernet.begin(macAddress);

	// give the Ethernet shield a second to initialize:
	delay(1000);
	
	return result == 1;
}

bool ServerComm::StartComm(IPAddress server, int port)
{
	// if you get a connection, report back via serial:
	if (client.connect(server, port)) {
		return true;
	} else {
		// if you didn't get a connection to the server:
		return false;
	}
}


bool ServerComm::SendMessage(MessageType type, float* ratio)
{
	// Alloc space for the message
	byte message[12];
	
	// Create version
	short version = MAYOR_NUM_VERSION_PACKET << 8 | MINOR_NUM_VERSION_PACKET;
	
	// Fill the header
	memcpy(message,&version,2);
	message[2] = (byte) type;
	message[3] = ratio ? 1 : 0;
	
	// Add the id to the message
	memcpy(message + 4,&id,4);
	
	// If there is custom data, add if to the message
	if(ratio)
	{
		memcpy(message + 8,ratio,4);
	}
	
	// Encrypt the message
	Cryptography::encrypt((unsigned long*) message, 3, key);
	
	// If there is a connection avaliable, send the message
	if(client.connected())
	{
		return client.write(message,12) == 12;
	}
	
	return false;
}