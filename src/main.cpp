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

//Modbus
#define RST 1
#define PORT 502

// Declaration for SSD1306 display connected using software SPI (default case):
#define OLED_DC    12
#define OLED_CS    13
#define OLED_RESET 14
#define OLED_CS2    6
#define OLED_RESET2 5

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

SSD1306Spi display(OLED_RESET, OLED_DC, OLED_CS);
SSD1306Spi display2(OLED_RESET2, OLED_DC, OLED_CS2);
PCF8574 pcf(PCF_ADRESS);
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(PWM_ADRESS);

//modbus
class ModbusEthernet : public ModbusAPI<ModbusTCPTemplate<EthernetServer, EthernetClient>> {};

IPAddress server(169, 254, 198, 12);  // Address of Modbus Slave device - need to define!!
const int32_t showDelay = 1000;   // Show result every n'th mellisecond

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEE }; // MAC address for your controller
IPAddress ip(169, 254, 198, 200); // The IP address will be dependent on your local network
ModbusEthernet mb;               // Declare ModbusTCP instance

void setup()
{
	Serial.begin(9600);
	delay(1000);

	// Set pinMode to OUTPUT
	pcf.pinMode(P0, INPUT);
  pcf.pinMode(P1, INPUT);

	Serial.print("Init pcf8574...");
	if (pcf.begin())
		Serial.println("OK");
  else
		Serial.println("KO");

  //PWM
  pwm.begin();
  pwm.setPWMFreq(50);

  String text1 = "Frequency: 50 Hz";
  String text2 = "Power: 600 kW";
  String text3 = "Voltage: 1000 V";
  // put your setup code here, to run once:
  display.init();
  display.flipScreenVertically();
  display.setContrast(255);
 
  display2.init();
  display2.flipScreenVertically();
  display2.setContrast(255);

  display.setFont(ArialMT_Plain_16);
  display.drawStringMaxWidth(0,0, display.getWidth(),text1);
  display.drawStringMaxWidth(0,20, display.getWidth(),text2);
  display.drawStringMaxWidth(0,40, display.getWidth(),text3);
  display.display();
  delay(1000);
  display.clear();
  display.setFont(ArialMT_Plain_10); 

  //W550 reset
  pinMode(RST, OUTPUT);
  digitalWrite(RST, HIGH);
  delay(100);
  digitalWrite(RST, LOW);
  delay(100);
  digitalWrite(RST, HIGH);
  delay(100);

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
  /*
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
  }*/
  mb.client();
}

boolean test1 = 0;
uint16_t test2 = 0;
uint16_t test3 = 0;
uint16_t test4 = 0;
uint16_t progress = 0;

uint32_t showLast = 0;

void loop()
{
	/*uint8_t state = read3State(P0, P1, true, pcf);
  if(state == 0)
    RGBLedColor(0, 255, 0, 0, pwm);
  else if(state == 1)
    RGBLedColor(0, 0, 255, 0, pwm);
  else if(state == 2)
    RGBLedColor(0, 0, 0, 255, pwm);
  delay(10);*/

  display.clear();

  /*
  for(uint8_t i = 0; i < 100; ++i)
  {
    display.drawProgressBar(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 60, 30, i);
    display.display();
    delay(20);
  }*/

  int16_t x0 = SCREEN_WIDTH/2;
  int16_t y0 = SCREEN_HEIGHT/2;
  int16_t r = 20;

  progress += 2; 
  if(progress > 100)
  {
    progress = 0;
    display2.clear();
    //display2.fillRect(0, 0, 128, 64);
    display2.drawXbm(0, 0, thruster_width, thruster_height, thruster_bits);
    display2.display();
    delay(3000);
  }
    
  dispPemsVisualize(display2, progress);
  display2.display();
  display2.clear();

  display.clear();
  display.drawXbm(0, 0, abbWidth, abbHeight, abbImg);
  display.display();

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

