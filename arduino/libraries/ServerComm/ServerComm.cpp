#include "ServerComm.h"
#include "Cryptography.h"

const unsigned long ServerComm::key[4] = { 56394, 12134315, 74, 156 };
const short ServerComm::version = MAYOR_NUM_VERSION_PACKET << 8 | MINOR_NUM_VERSION_PACKET;

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
	MessageHeader outMessage = {version, (byte)type, (byte)(ratio ? 1 : 0), id, messageId++, *ratio};

	// Encrypt the message
	Cryptography::encrypt((unsigned long*) &outMessage, 4, key);
  
	// If there is a connection avaliable, send the message
	if(client.connected())
	{
		return client.write((byte*)&outMessage,16) == 16;
	}
  
	return false;
	/*
	// Alloc space for the message
	byte message[16];
	
	// Fill the header
	memcpy(message,&version,2);
	message[2] = (byte) type;
	message[3] = ratio ? 1 : 0;
	
	// Add the id to the message
	memcpy(message + 4,&id,4);

  // Add the messageID to the message
  memcpy(message + 8,&messageId,4);
  messageId++;
  
	// If there is custom data, add if to the message
	if(ratio)
	{
		memcpy(message + 12,ratio,4);
	}

	// Encrypt the message
	Cryptography::encrypt((unsigned long*) message, 4, key);
	
	// If there is a connection avaliable, send the message
	if(client.connected())
	{
		return client.write(message,16) == 16;
	}
	
	return false;
 */
}

bool ServerComm::ReceiveMessage(MessageHeader& inMessage)
{
  if(client.available() < 12)
  {
    return false;
  }
  byte inBytes[12];
  for(int i = 0; i < 12; i++)
  {
    inBytes[i] = client.read();
  }

  memcpy(&inMessage, inBytes, 12);

  // Decrypt the message
  Cryptography::decrypt((unsigned long*) &inMessage, 3, key);

  if(inMessage.version != version)
  {
    return false;
  }

  return true;
}

