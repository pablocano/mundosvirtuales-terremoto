#include <Wire.h>
#include <SPI.h>
#include <Ethernet.h>
#include <ServerComm.h>
#include <MsTimer2.h>
#include <EEPROM.h>
#define BMA180 0x40  //address of the accelerometer
#define RESET 0x10   
#define PWR 0x0D
#define BMA180_BW_BANDPASS 0x09
#define RANGE 0X35
#define DATA 0x02 
#define TO_READ (6)      //num of bytes we are going to read each time (two bytes for each axis)
#define SERVER_ADDR "plant.mundos-virtuales.com"
//#define SERVER_ADDR "192.168.1.140"
#define MEM_ADDR_ID_DEVICE 0
#define WAIT_RESPONSE 150
// 
byte buff[TO_READ] ;    //6 bytes buffer for saving data read from the device
//char str[512];                      //string buffer to transform data before sending it to the serial port
//
int offx = 31;  
int offy = 47;   
int offz = -23;
//
const int numReading1 = 10; //LTA
const int numReading2 = 8; //STA
const int ledPin = 13; // (buzzer)
int reading1[numReading1] = {}; // Initialize to zero
int reading2[numReading2] = {}; // Initialize to zero
int readIndex1 = 0;
int readIndex2 = 0;
int total1 = 0;
int total2 = 0;
int LTA = 0;
int STA = 0;
int ratio = 0;
//
byte mac[] = { 0x0E, 0xAD, 0x00, 0x00, 0x00, 0x00 };
int port = 4322;
long idDevice = 0L;
//
ServerComm comm;
//
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
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output
  Serial.println("Sensor demo started");
  connectToServer();
  Serial.println("Initializing sensors");
  AccelerometerInit();
  for (int thisReading1 = 0; thisReading1 < numReading1; thisReading1++)
  {
    reading1[thisReading1] = 0;
  }
  for (int thisReading2 = 0; thisReading2 < numReading2; thisReading2++)
  {
    reading2[thisReading2] = 0;
  }
  Serial.println("Sensors have been initialized");
  // Set timer to maintain the connection with the server
  MsTimer2::set(60000, alive); // 60s period
  MsTimer2::start();
}
//
void AccelerometerInit() 
{ 
 byte temp[1];
 byte temp1;
  //
  writeTo(BMA180,RESET,0xB6);
  //wake up mode
  writeTo(BMA180,PWR,0x90);
  temp1=temp[0]&0x10;
  // range +/- 2g 
  readFrom(BMA180, RANGE, 1 ,temp);  
  temp1=(temp[0]&0xF1) | 0x04;
  writeTo(BMA180,RANGE,temp1);
}
//
void AccelerometerRead()
{
 int n=6;
 byte result[5];
 readFrom(BMA180, DATA, n , result);
 
 int x= (( result[0] | result[1]<<8)>>2)+offx;
 int y= (( result[0] | result[1]<<8)>>2)+offx;
 int z= (( result[0] | result[1]<<8)>>2)+offx;
 float h=abs(z/4096.0);
  
   //LTA:
   total1 = total1 - reading1[readIndex1];
   reading1[readIndex1] = abs(z);
   total1 = total1 + reading1[readIndex1];
   readIndex1 = readIndex1 + 1;
   if (readIndex1 >= numReading1); {
   readIndex1 = 0;
   }
   LTA = total1/numReading1;
   delay (1);

  //STA:
   total2 = total2 - reading2[readIndex2];
   reading2[readIndex2] = abs(z);
   total2 = total2 + reading2[readIndex2];
   readIndex2 = readIndex2 + 1;
   if (readIndex2 >= numReading2); {
   readIndex2 = 0;
   }
   STA = total2/numReading2;
   delay (1);

  //ratio LTA/STA

 ratio = STA/LTA;
  
 Serial.print("h=");
 Serial.print(h);
 Serial.print("\t");

 Serial.print("LTA=");
 Serial.print(LTA);
 Serial.print("\t");

 Serial.print("STA=");
 Serial.print(STA);
 Serial.print("\t");
 
 Serial.print("LTA/STA=");
 Serial.print(ratio);
 Serial.print("\n");

  
  if(!comm.Connected())
  {
    Serial.println("Reconnecting ...");
    comm.StopClient();
    connectToServer();
  }
  
  if (ratio > 1.00)
  {
    Serial.println("EARTHQUAKE DETECTED");
    digitalWrite(ledPin, HIGH);
    if(comm.SendMessage(ServerComm::EARTHQUAKE, &h))
    {
      Serial.println("Message sent successfully");
    }
    delay(10000); // TODO: this is necessary?
    //software_Reset();
  } 
  else
  {
    digitalWrite(ledPin, LOW);
  }
}
void software_Reset() // Restarts program from beginning but does not reset the peripherals and registers
{
asm volatile ("jmp 0");
}
void loop() 
{ 
 AccelerometerRead(); 
}

//---------------- Functions--------------------
//Writes val to address register on ACC
void writeTo(int DEVICE, byte address, byte val) 
{
  Wire.beginTransmission(DEVICE);   //start transmission to ACC
  Wire.write(address);               //send register address
  Wire.write(val);                   //send value to write
  Wire.endTransmission();           //end trnsmisson
}
//reads num bytes starting from address register in to buff array
 void readFrom(int DEVICE, byte address , int num ,byte buff[])
 {
 Wire.beginTransmission(DEVICE); //start transmission to ACC
 Wire.write(address);            //send reguster address
 Wire.endTransmission();        //end transmission
 
 Wire.beginTransmission(DEVICE); //start transmission to ACC
 Wire.requestFrom(DEVICE,num);  //request 6 bits from ACC
 
  int i = 0;
  while(Wire.available())    //device may send less than requested (abnormal)
  { 
    buff[i] = Wire.read(); // receive a byte
    i++;
  }
  Wire.endTransmission(); //end transmission
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



