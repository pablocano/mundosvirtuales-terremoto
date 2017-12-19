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

bool ServerComm::StartComm(char* server, int port)
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
}

bool ServerComm::ReceiveMessage(MessageHeader& inMessage)
{
  if(client.available() < 16)
  {
    return false;
  }
  byte inBytes[16];
  for(int i = 0; i < 16; i++)
  {
    inBytes[i] = client.read();
  }

  memcpy(&inMessage, inBytes, 16);

  // Decrypt the message
  Cryptography::decrypt((unsigned long*) &inMessage, 4, key);

  if(inMessage.version != version)
  {
    return false;
  }

  return true;
}

