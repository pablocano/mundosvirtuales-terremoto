#include <Ethernet.h>
#include <ServerComm.h>
#include <MsTimer2.h>

byte mac[] = { 0x0E, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
int port = 4323;

ServerComm::MessageHeader inMessage;

ServerComm comm(3L);

void alive()
{
    Ethernet.maintain();
    if(comm.SendMessage(ServerComm::ALIVE))
    {
      Serial.println("Sending alive"); 
    }
}

void setup()
{
  Serial.begin(9600);  // start serial for output

  Serial.println("Demo started");

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
    if(inMessage.type == (byte)ServerComm::EARTHQUAKE)
    {
      Serial.println("Earthquake detected");
      digitalWrite(13, HIGH);
      onAlarm();
      delay(3000);
    }
  }

  delay(10);

  if(!comm.Connected())
  {
    Serial.println("Reconnecting");
    comm.StopClient();
    connectToServer();
  } 
}

void onAlarm()
{
  tone(8, 262, 1000 / 4);
}

void connectToServer()
{
  if(comm.Begin(mac))
  {
    Serial.println("DHCP connection success");
  }
  
  Serial.println("Connecting ...");
  while(!comm.StartComm("plant.mundos-virtuales.com", port))
  {
    Serial.println("Failed. Trying again ...");
    delay(2000);
  }
  Serial.println("Connection Established");
}



