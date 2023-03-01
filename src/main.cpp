#include <Wire.h>
#include <Arduino.h>
#include <PCF8574.h>
#include <stdbool.h>
#include <Adafruit_PWMServoDriver.h>
#include "../lib/Marine_panel.h"
#include <SPI.h>
#include "SSD1306Spi.h"
#include <Ethernet.h>       // Ethernet library v2 is required
#include <ModbusAPI.h>
#include <ModbusTCPTemplate.h>




//peripherals
//PCF8574 pcf(PCF_ADRESS); DAMAGED needs to be replaced
PCF8574 pcf1(PCF2_ADRESS); //PCF2 Adress just for test purposes
PCF8574 pcf2(PCF3_ADRESS); //PCF3 Adress just for test purposes


SSD1306Spi display1(P0, DISP_DC, DISP1_CS, &pcf1, true);
SSD1306Spi display2(P1, DISP_DC, DISP2_CS, &pcf1, true);
SSD1306Spi display3(P2, DISP_DC, DISP3_CS, &pcf1, true);
SSD1306Spi display4(P3, DISP_DC, DISP4_CS, &pcf1, true);
SSD1306Spi display5(P4, DISP_DC, DISP5_CS, &pcf1, true);
SSD1306Spi display6(P5, DISP_DC, DISP6_CS, &pcf1, true);
SSD1306Spi display7(P6, DISP_DC, DISP7_CS, &pcf1, true);
SSD1306Spi display8(P7, DISP_DC, DISP8_CS, &pcf1, true);
SSD1306Spi display9(P0, DISP_DC, DISP9_CS, &pcf2, true);


Adafruit_PWMServoDriver pwm1 = Adafruit_PWMServoDriver(PWM1_ADRESS);

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

	pcfAllOutInit(pcf1);
  pcfAllOutInit(pcf2);
  pwmInit(pwm1);


  String text1 = "Frequency: 50 Hz";
  String text2 = "Power: 600 kW";
  String text3 = "Voltage: 1000 V";


  dispInit(display1);
  dispInit(display2);
  dispInit(display3);
  dispInit(display4);
  dispInit(display5);
  dispInit(display6);
  dispInit(display7);
  dispInit(display8);
  dispInit(display9);

  


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
uint8_t state = 0;

uint32_t showLast = 0;


void loop()
{

  display1.drawString(0, 0, "Display 1");
  display1.display();
  display2.drawString(0, 0, "Display 2");
  display2.display();
  display3.drawString(0, 0, "Display 3");
  display3.display();
  display4.drawString(0, 0, "Display 4");
  display4.display();
  display5.drawString(0, 0, "Display 5");
  display5.display();
  display6.drawString(0, 0, "Display 6");
  display6.display();
  display7.drawString(0, 0, "Display 7");
  display7.display();
  display8.drawString(0, 0, "Display 8");
  display8.display();
  display9.drawString(0, 0, "Display 9");
  display9.display();

  RGBLedColor(0, 255, 0, 0, pwm1);
  RGBLedColor(3, 255, 0, 0, pwm1);
  RGBLedColor(6, 255, 0, 0, pwm1);
  delay(1000);

  RGBLedColor(0, 0, 255, 0, pwm1);
  RGBLedColor(3, 0, 255, 0, pwm1);
  RGBLedColor(6, 0, 255, 0, pwm1);
  delay(1000);

  RGBLedColor(0, 0, 0, 255, pwm1);
  RGBLedColor(3, 0, 0, 255, pwm1);
  RGBLedColor(6, 0, 0, 255, pwm1);
  delay(1000);

  



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
  
}

