#include <Ethernet.h>
#include <ServerComm.h>
#include <MsTimer2.h>
#include <EEPROM.h>

//#define SERVER_ADDR "plant.mundos-virtuales.com"
#define SERVER_ADDR "192.168.1.140"

#define MEM_ADDR_ID_DEVICE 0
#define WAIT_RESPONSE 150

ServerComm::MessageHeader inMessage;

long idDevice = 0L;

ServerComm comm;

byte mac[] = { 0x0E, 0xAA, 0x00, 0x00, 0x00, 0x00 };
int port = 4323;

void alive()
{
    Ethernet.maintain();
    if(comm.SendMessage(ServerComm::ALIVE))
    {
      Serial.print("Sending alive, ID: ");
      Serial.println(idDevice, DEC);
    }
}

void setup()
{
  // Uncomment below line for reseting identification device
  // EEPROM.put(MEM_ADDR_ID_DEVICE, -1L);

  // Recovery identification of device
  EEPROM.get(MEM_ADDR_ID_DEVICE, idDevice);
  
  Serial.begin(9600);  // start serial for output

  Serial.println("Alarm demo started");

  connectToServer();

  // Set timer to maintain the connection with the server
  MsTimer2::set(60000, alive); // 60s period
  MsTimer2::start();
}

void loop()
{
  digitalWrite(13, LOW);
  if(comm.ReceiveMessage(inMessage))
  {
    if(inMessage.m_command == (byte)ServerComm::EARTHQUAKE)
    {
      Serial.println("Earthquake detected");
      digitalWrite(13, HIGH);
      onAlarm();
    }
  }
  
  delay(10);

  if(!comm.Connected())
  {
    Serial.println("Reconnecting ...");
    comm.StopClient();
    connectToServer();
  } 
}

void onAlarm()
{
  tone(8, 262, 1000);
}

void stopAlarm()
{
  noTone(8);
}

void connectToServer()
{
  // Generate Mac from identification of device
  mac[2] = (byte) ((idDevice >> 24) & 0xFF);
  mac[3] = (byte) ((idDevice >> 16) & 0xFF);
  mac[4] = (byte) ((idDevice >>  8) & 0xFF);
  mac[5] = (byte) ((idDevice >>  0) & 0xFF);
    
  if(comm.Begin(mac))
  {
    Serial.println("DHCP connection success");
  }
  
  Serial.println("Connecting ...");
  
  while(true)
  {
    while(!comm.StartComm(SERVER_ADDR, port))
    {
      Serial.println("Failed. Trying again ...");
      delay(2000);
    }

    if(ackServer())
    {
      break;
    }
    else
    {
      comm.StopClient();
      delay(500);
    }
  }
  
  Serial.println("Connection Established");

}

bool ackServer()
{
  ServerComm::MessageHeader inMessage;

  // Waiting for response
  delay(WAIT_RESPONSE);
  Serial.println("Ack Server");
  // Receive response from server
  if(comm.ReceiveMessage(inMessage, false) && inMessage.m_command == ServerComm::ACKNOWLEDGE_CLIENT)
  {
    Serial.print("id: ");
    Serial.println(idDevice, DEC);

    comm.setID(idDevice); // Setting identification

    if(comm.SendMessage(ServerComm::ACKNOWLEDGE_CLIENT))
    {
      if(isValidID(idDevice))
      {
        return true;
      }
      else
      {
        Serial.println("Renew Identification.");
        // Waiting for response
        delay(WAIT_RESPONSE);
        // Receive response from server
        if(comm.ReceiveMessage(inMessage, false) && inMessage.m_command == ServerComm::RENEW_DEVICE_ID)
        {
          idDevice = (long) inMessage.m_payload; // TODO: copy memory
          Serial.print("Renew id: ");
          Serial.println(idDevice, DEC);
          if(isValidID(idDevice))
          {
            comm.setID(idDevice); // Setting identification
            EEPROM.put(MEM_ADDR_ID_DEVICE, idDevice); // Store identification on EEPROM
            
            Serial.println("The identification of sensor was successful renewed.");

            Serial.println("Send confirmation renew packet.");
            return comm.SendMessage(ServerComm::ACKNOWLEDGE_CLIENT);
          }
        }
        else
        {
          Serial.println("Problem receive renew packet.");
        }
      }
    } 
  }

  return false;
}

bool isValidID(long id)
{
  return id > 0L;
}



