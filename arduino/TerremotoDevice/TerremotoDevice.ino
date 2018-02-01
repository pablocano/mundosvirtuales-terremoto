#include <Wire.h>
#include <SPI.h>
#include <Ethernet.h>
#include <ServerComm.h>
#include <MsTimer2.h>
#include <EEPROM.h>
 
#define DEVICE (0x53)    //ADXL345 device address
#define TO_READ (6)      //num of bytes we are going to read each time (two bytes for each axis)

//#define SERVER_ADDR "plant.mundos-virtuales.com"
#define SERVER_ADDR "192.168.1.140"

#define MEM_ADDR_ID_DEVICE 0
#define WAIT_RESPONSE 150
 
byte buff[TO_READ] ;    //6 bytes buffer for saving data read from the device
//char str[512];                      //string buffer to transform data before sending it to the serial port

const int numReading1 = 10; //LTA
const int numReading2 = 8; //STA
const int ledPin = 13; // (buzzer)
int reading1[numReading1] = {}; // Initialize to zero
int reading2[numReading2] = {}; // Initialize to zero
int readIndex1 = 0;
int readIndex2 = 0;
int total1 = 0;
int total2 = 0;
int average = 0;

byte mac[] = { 0x0E, 0xAD, 0x00, 0x00, 0x00, 0x00 };
int port = 4322;

long idDevice = 0L;

ServerComm comm;

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
  for (int thisReading1 = 0; thisReading1 < numReading1; thisReading1++)
  {
    reading1[thisReading1] = 0;
  }
  for (int thisReading2 = 0; thisReading2 < numReading2; thisReading2++)
  {
    reading2[thisReading2] = 0;
  }
 
  //Turning on the ADXL345  
  writeTo(DEVICE, 0x2D, 0);      
  writeTo(DEVICE, 0x2D, 16);
  writeTo(DEVICE, 0x2D, 8);

  Serial.println("Sensors have been initialized");

  // Set timer to maintain the connection with the server
  MsTimer2::set(60000, alive); // 60s period
  MsTimer2::start();
}
 
void loop()
{
  int regAddress = 0x32;    //first axis-acceleration-data register on the ADXL345
  int x, y, z;
  int average;
   
  readFrom(DEVICE, regAddress, TO_READ, buff); //read the acceleration data from the ADXL345
 
  //each axis reading comes in 10 bit resolution, ie 2 bytes.  Least Significat Byte first!!
  //thus we are converting both bytes in to one int
  x = (((int)buff[1]) << 8) | buff[0];   
  y = (((int)buff[3]) << 8) | buff[2];
  z = (((int)buff[5]) << 8) | buff[4];
 
  //we send the x y z values as a string to the serial port
  //sprintf(str, "%d, %d, %d", x, y, z);
  average = abs((x+y/2));
  
  //LTA:
  total1 = total1 - reading1[readIndex1];
  reading1[readIndex1] = (average);
  total1 = total1 + reading1[readIndex1];
  readIndex1 = readIndex1 + 1;
  (readIndex1 >= numReading1); {
    readIndex1 = 0;
  }
   int LTA = abs(total1/numReading1);
   delay (1);

  //STA:
  total2 = total2 - reading2[readIndex2];
  reading2[readIndex2] = (average);
  total2 = total2 + reading2[readIndex2];
  readIndex2 = readIndex2 + 1;
  (readIndex2 >= numReading2); {
    readIndex2 = 0;
  }
  int STA = abs(total2/numReading2);
  delay (1);

  //ratio LTA/STA

  float ratio = abs(STA/LTA);
  
  //Serial.print("LTA/STA=");
  //Serial.print(abs(ratio));
  //Serial.print("\n");

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
    if(comm.SendMessage(ServerComm::EARTHQUAKE, &ratio))
    {
      Serial.println("Message sent successfully");
    }
    delay(1000); // TODO: this is necessary?
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

//---------------- Functions
//Writes val to address register on device
void writeTo(int device, byte address, byte val) {
   Wire.beginTransmission(device); //start transmission to device 
   Wire.write(address);        // send register address
   Wire.write(val);        // send value to write
   Wire.endTransmission(); //end transmission
}
 
//reads num bytes starting from address register on device in to buff array
 
void readFrom(int device, byte address, int num, byte buff[]) 
{
  Wire.beginTransmission(device); //start transmission to device 
  Wire.write(address);        //sends address to read from
  Wire.endTransmission(); //end transmission
 
  Wire.beginTransmission(device); //start transmission to device
  Wire.requestFrom(device, num);    // request 6 bytes from device
 
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



