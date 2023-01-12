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

const uint16_t REG = 512;               // Modbus Hreg Offset
IPAddress server(127, 0, 0, 1);  // Address of Modbus Slave device - need to define!!
const int32_t showDelay = 5000;   // Show result every n'th mellisecond

bool usingDhcp = true;
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEE }; // MAC address for your controller
IPAddress ip(169, 254, 198, 200); // The IP address will be dependent on your local network
ModbusEthernet mb;               // Declare ModbusTCP instance



void setup() {
  
    Serial.begin(9600);
    uint32_t timeout = 5000;
    uint32_t startTime = millis();
    while (!Serial && millis() - startTime < timeout)
        continue;

    pinMode(RST, OUTPUT);
    digitalWrite(RST, HIGH);
    delay(100);
    digitalWrite(RST, LOW);
    delay(100);
    digitalWrite(RST, HIGH);
    delay(100);
    Ethernet.init(34);


    Ethernet.begin(mac, ip);
    //server.begin();
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
    if (mb.isConnected(server))
      Serial.println("connected");     
    else
      Serial.println("Connection failed");
}

uint16_t res = 0;
uint32_t showLast = 0;


void loop() {
if (mb.isConnected(server)) {   // Check if connection to Modbus Slave is established
    mb.readHreg(server, REG, &res);  // Initiate Read Hreg from Modbus Slave
  } else {
    mb.connect(server, PORT);           // Try to connect if not connected
  }

  delay(100);                     // Pulling interval
  mb.task();                      // Common local Modbus task
  if (millis() - showLast > showDelay) { // Display register value every 5 seconds (with default settings)
    showLast = millis();
    Serial.println(res);
  }
}





