#include <Wire.h>
#include <Arduino.h>
#include <PCF8574.h>
#include <stdbool.h>
#include <Adafruit_PWMServoDriver.h>
#include "../lib/Marine_panel.h"
#include <Adafruit_SSD1306.h>
#include <SPI.h> 
#include <Ethernet.h>       // Ethernet library v2 is required
#include <ModbusAPI.h>
#include <ModbusTCPTemplate.h>




//peripherals
PCF8574 pcf1(PCF1_ADRESS); 
PCF8574 pcf2(PCF2_ADRESS); 

/*
SSD1306Spi display1(P0, 39, DISP1_CS, &pcf1, true, true);
SSD1306Spi display2(P1, 39, DISP2_CS, &pcf1, true, true);
SSD1306Spi display3(P2, 39, DISP3_CS, &pcf1, true, true);
SSD1306Spi display4(P3, 39, DISP4_CS, &pcf1, true, true);*/
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for SSD1306 display connected using software SPI (default case):
#define OLED_MOSI   19
#define OLED_CLK   20
#define OLED_DC    18
#define OLED_CS    39 //++
#define OLED_CS2    0 //++
#define OLED_RESET 17 //--

Adafruit_SSD1306 display1(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);
Adafruit_SSD1306 display2(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS+1);
Adafruit_SSD1306 display3(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS+2);
Adafruit_SSD1306 display4(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS+3);
Adafruit_SSD1306 display5(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS+4);
Adafruit_SSD1306 display6(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS+5);
Adafruit_SSD1306 display7(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS+6);


Adafruit_SSD1306 display8(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS2);
Adafruit_SSD1306 display9(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS2+1);
Adafruit_SSD1306 display10(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS2+2);
Adafruit_SSD1306 display11(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS2+3);
Adafruit_SSD1306 display12(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS2+4);
Adafruit_SSD1306 display13(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS2+5);
Adafruit_SSD1306 display14(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS2+6);
Adafruit_SSD1306 display15(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS2+7);

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16
static const unsigned char PROGMEM logo_bmp[] =
{ 0b00000000, 0b11000000,
  0b00000001, 0b11000000,
  0b00000001, 0b11000000,
  0b00000011, 0b11100000,
  0b11110011, 0b11100000,
  0b11111110, 0b11111000,
  0b01111110, 0b11111111,
  0b00110011, 0b10011111,
  0b00011111, 0b11111100,
  0b00001101, 0b01110000,
  0b00011011, 0b10100000,
  0b00111111, 0b11100000,
  0b00111111, 0b11110000,
  0b01111100, 0b11110000,
  0b01110000, 0b01110000,
  0b00000000, 0b00110000 };

  void writeDisp(Adafruit_SSD1306 &display)
  {
    display.setTextSize(1); // Draw 2X-scale text
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(10, 0);
    display.println(F("Hello world"));
    display.display();
  }

  void dispInit(Adafruit_SSD1306 &display, bool reset)
  {
    if(reset)
    {
      if(!display.begin(SSD1306_SWITCHCAPVCC, 0, true)) 
      {
        Serial.println(F("SSD1306 allocation failed"));
        for(;;); // Don't proceed, loop forever
      }
    }
    else
    {
      if(!display.begin(SSD1306_SWITCHCAPVCC, 0, false)) 
      {
        Serial.println(F("SSD1306 allocation failed"));
        for(;;); // Don't proceed, loop forever
      }
    }
    
  }



Adafruit_PWMServoDriver pwm1 = Adafruit_PWMServoDriver(PWM1_ADRESS);
Adafruit_PWMServoDriver pwm2 = Adafruit_PWMServoDriver(PWM2_ADRESS);
Adafruit_PWMServoDriver pwm3 = Adafruit_PWMServoDriver(PWM3_ADRESS);

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
  pwmInit(pwm2);
  pwmInit(pwm3);


  String text1 = "Frequency: 50 Hz";
  String text2 = "Power: 600 kW";
  String text3 = "Voltage: 1000 V";

  delay(1000);
  
  dispInit(display1, true);
  dispInit(display2, false);
  dispInit(display3, false);
  dispInit(display4, false);
  dispInit(display5, false);
  dispInit(display6, false);
  dispInit(display7, false);
  dispInit(display8, false);
  dispInit(display9, false);
  dispInit(display10, false);
  dispInit(display11, false);
  dispInit(display12, false);
  dispInit(display13, false);
  dispInit(display14, false);
  dispInit(display15, false);

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display1.display();
  display2.display();
  display3.display();
  display4.display();
  display5.display();
  display6.display();
  display7.display();
  display8.display();
  display9.display();
  display10.display();
  display11.display();
  display12.display();
  display13.display();
  display14.display();
  display15.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display1.clearDisplay();
  display2.clearDisplay();
  display3.clearDisplay();
  display4.clearDisplay();
  display5.clearDisplay();
  display6.clearDisplay();
  display7.clearDisplay();
  display8.clearDisplay();
  display9.clearDisplay();
  display10.clearDisplay();
  display11.clearDisplay();
  display12.clearDisplay();
  display13.clearDisplay();
  display14.clearDisplay();
  display15.clearDisplay();

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
   uint16_t timeDel = 300; 
  writeDisp(display1);
  delay(timeDel);
  writeDisp(display2);
  delay(timeDel);
  writeDisp(display3);
  delay(timeDel);
  writeDisp(display4);
  delay(timeDel);
  writeDisp(display5);
  delay(timeDel);
  writeDisp(display6);
  delay(timeDel);
  writeDisp(display7);
  delay(timeDel);
  writeDisp(display8);
  delay(timeDel);
  writeDisp(display9);
  delay(timeDel);
  writeDisp(display10);
  delay(timeDel);
  writeDisp(display11);
  delay(timeDel);
  writeDisp(display12);
  delay(timeDel);
  writeDisp(display13);
  delay(timeDel);
  writeDisp(display14);
  delay(timeDel);
  writeDisp(display15);
  delay(timeDel);
/*
  RGBLedTest(5, pwm1);
  RGBLedTest(5, pwm2);
  RGBLedTest(4, pwm3);

  int analogData  = map(analogRead(20), 0, 8191, 0, 100);
  int analogData2  = map(analogRead(19), 0, 8191, 0, 100);
  Serial.println(analogData);
  Serial.println(analogData2);
  //Serial.println(analogData4);
  delay(500);

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
  */
}

