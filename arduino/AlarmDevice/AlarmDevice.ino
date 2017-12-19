#include <Ethernet.h>
#include <ServerComm.h>
#include <MsTimer2.h>

byte mac[] = { 0x0E, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
int port = 4323;

ServerComm::MessageHeader inMessage;

ServerComm comm(3L);
IPAddress server(192, 168, 1, 140);

void alive()
{
    Ethernet.maintain();
    comm.SendMessage(ServerComm::ALIVE);
    Serial.println("Sending alive");
}

void setup()
{
  Serial.begin(9600);  // start serial for output

  Serial.println("Demo started");

  onAlarm();

  if(comm.Begin(mac))
  {
    Serial.println("DHCP connection success");
  }
  
  Serial.println("Connecting ...");
  while(!comm.StartComm(server, port))
  {
    delay(1000);
    Serial.println("Failed. Trying again ...");
  }
  Serial.println("Connection Established");

  // Set timer to maintain the connection with the server
  MsTimer2::set(60000, alive); // 60s period
  MsTimer2::start();
}

void loop()
{
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
}

void onAlarm()
{
  tone(8, 262, 1000 / 4);
}



