#include <Wire.h>
#include <Arduino.h>
#include <PCF8574.h>
#include <stdbool.h>
#include <Adafruit_PWMServoDriver.h>
#include "../lib/Marine_panel.h"
#include <SPI.h>
//#include "SSD1306Spi.h"
#include <Ethernet.h>       // Ethernet library v2 is required
#include <ModbusAPI.h>
#include <ModbusTCPTemplate.h>




//peripherals
PCF8574 pcf(PCF_ADRESS);
SSD1306Spi display(P0, DISP_DC, DISP1_CS, &pcf, true);
SSD1306Spi display2(P1, DISP_DC, DISP2_CS, &pcf, true);
SSD1306Spi display3(P2, DISP_DC, DISP3_CS, &pcf, true);
SSD1306Spi display4(P3, DISP_DC, DISP4_CS, &pcf, true);

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(PWM_ADRESS);

//modbus
class ModbusEthernet : public ModbusAPI<ModbusTCPTemplate<EthernetServer, EthernetClient>> {};
IPAddress server(169, 254, 198, 12);  // Address of Modbus Slave device - need to define!!
const int32_t showDelay = 1000;   // Show result every n'th mellisecond
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEE }; // MAC address for your controller
IPAddress ip(169, 254, 198, 200); // The IP address will be dependent on your local network
ModbusEthernet mb;               // Declare ModbusTCP instance
boolean mbOn = false;

void setup()
{
	Serial.begin(9600);
	delay(1000);

	pcfInit(pcf);
  pwmInit(pwm);


  String text1 = "Frequency: 50 Hz";
  String text2 = "Power: 600 kW";
  String text3 = "Voltage: 1000 V";

  dispInit(display);
  dispInit(display2);
  dispInit(display3);
  dispInit(display4);


  display4.setFont(ArialMT_Plain_16);
  display4.drawStringMaxWidth(0,0, display.getWidth(),text1);
  display4.drawStringMaxWidth(0,20, display.getWidth(),text2);
  display4.drawStringMaxWidth(0,40, display.getWidth(),text3);
  display4.display();
  delay(1000);
  display.clear();
  display.setFont(ArialMT_Plain_10); 

  display3.setFont(ArialMT_Plain_16);
  display3.drawStringMaxWidth(0,0, display.getWidth(),text1);
  display3.display();
  delay(1000);
  W5500Reset(); //needed?


  if(mbOn)
  {
    //Modbus
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

  //joystick test

}
  

boolean test1 = 0;
uint16_t test2 = 0;
uint16_t test3 = 0;
uint16_t test4 = 0;
uint16_t progress = 0;

uint32_t showLast = 0;


void loop()
{

 
	uint8_t state = read3State(P0, P1, false, pcf);
  if(state == 0)
    RGBLedColor(0, 255, 0, 0, pwm);
  else if(state == 1)
  {
    RGBLedColor(0, 0, 255, 0, pwm);
    mb.writeCoil(server, 1, true);
    mb.writeHreg(server, 2, 100);
  }
    
  else if(state == 2)
  {
    RGBLedColor(0, 0, 0, 255, pwm);
    mb.writeCoil(server, 1, false);
    mb.writeHreg(server, 2, 200);
  }
    
  delay(30);
  dispRCSAzipodVisualize(display, display2, display3);
  //dispAlarmVisualize(display, display2, display3, display4);



  /*display2.clear();
  display2.drawXbm(0, 0, thrustWithcircle10_width, thrustWithcircle10_height, thrustWithcircle10);
  display2.display();
  delay(1000);*/

  if(mbOn)
  {
    while (Ethernet.linkStatus() == LinkOFF) 
    {
      Serial.println("The Ethernet cable is unplugged...");
      delay(1000);
    }

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
  
  

  //joystick test
  //analogReadResolution(12);
  int analogData  = map(analogRead(20), 0, 8191, 0, 100);
  int analogData2  = map(analogRead(19), 0, 8191, 0, 100);
  Serial.println(analogData);
  Serial.println(analogData2);
  //Serial.println(analogData4);
  delay(500);

}

