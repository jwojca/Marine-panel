/*
  ModbusTCP Client for ClearCode Arduino wrapper

  (c)2021 Alexander Emelianov (a.m.emelianov@gmail.com)
  https://github.com/emelianov/modbus-esp8266

  This code is licensed under the BSD New License. See LICENSE.txt for more info.
*/

#define RST 1
#define PORT 502

#include <Ethernet.h>       // Ethernet library v2 is required
#include <ModbusAPI.h>
#include <ModbusTCPTemplate.h>



class ModbusEthernet : public ModbusAPI<ModbusTCPTemplate<EthernetServer, EthernetClient>> {};

IPAddress server(169, 254, 198, 12);  // Address of Modbus Slave device - need to define!!
const int32_t showDelay = 1000;   // Show result every n'th mellisecond

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEE }; // MAC address for your controller
IPAddress ip(169, 254, 198, 200); // The IP address will be dependent on your local network
ModbusEthernet mb;               // Declare ModbusTCP instance


void setup() {
  
    Serial.begin(9600);
    uint32_t timeout = 5000;
    uint32_t startTime = millis();
    while (!Serial && millis() - startTime < timeout)
        continue;

    /*pinMode(RST, OUTPUT);
    digitalWrite(RST, HIGH);
    delay(100);
    digitalWrite(RST, LOW);
    delay(100);
    digitalWrite(RST, HIGH);
    delay(100);*/

    Ethernet.init(34);
    Ethernet.begin(mac, ip);
    Serial.print("CLient IP adress: ");
    Serial.println(Ethernet.localIP());

    delay(1000); //second to initialize

    // Make sure the physical link is up before continuing.
    while (Ethernet.linkStatus() == LinkOFF) {
        Serial.println("The Ethernet cable is unplugged...");
        delay(1000);
    }
    Serial.println("The Ethernet cable is connected.");
    Serial.println("Client connecting to Server");
    mb.connect(server, PORT);
    delay(1000);
    while(1)
    {
      if (mb.isConnected(server))
      {
        Serial.println("Successfully connected"); 
        break;  
      }
      else
      {
        Serial.println("Connection failed, attempting to reconnect.");
        mb.connect(server, PORT);
      }
        
    }
    
    mb.client();
}

boolean test1 = 0;
uint16_t test2 = 0;
uint16_t test3 = 0;
uint16_t test4 = 0;

uint32_t showLast = 0;


void loop() {
  if (mb.isConnected(server)) 
  {  
      if(mb.readCoil(server, 0, &test1) == 0)
        Serial.print("Transaction 0");
      mb.readHreg(server, 0, &test2); 
      mb.readHreg(server, 1, &test3); 
      mb.readHreg(server, 100, &test4); 
      
    } 
  else 
  {
      mb.connect(server);           // Try to connect if not connected
  }
  delay(100);                     // Pulling interval
  mb.task();                      // Common local Modbus task
  if (millis() - showLast > showDelay)
  { // Display register value every x seconds (with default settings)
    showLast = millis();
    Serial.print("Test1: ");
    Serial.println(test1);
    Serial.print("Test2: ");
    Serial.println(test2);
    Serial.print("Test3: ");
    Serial.println(test3);
    Serial.print("Test4: ");
    Serial.println(test4);
  }
}



