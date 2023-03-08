#include <Wire.h>
#include <Arduino.h>
#include <PCF8574.h>
#include <stdbool.h>
#include <Adafruit_PWMServoDriver.h>
#include "../lib/Marine_panel/Marine_panel_v2.h"


#include <SPI.h> 
#include <Ethernet.h>       // Ethernet library v2 is required
#include <ModbusAPI.h>
#include <ModbusTCPTemplate.h>




//peripherals
PCF8574 pcf1(PCF1_ADRESS); 
PCF8574 pcf2(PCF2_ADRESS); 
PCF8574 pcf3(PCF3_ADRESS); 
PCF8574 pcf4(PCF4_ADRESS); 


// Declaration for SSD1306 display connected using software SPI (default case):
/*#define OLED_MOSI   19
#define OLED_CLK   20
#define OLED_DC    18
#define OLED_CS    39 //++
#define OLED_CS2    0 //++
#define OLED_RESET 17 //--*/

Adafruit_SSD1306 display1(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, DISP_DC, DISP_RESET, DISP1_CS);
Adafruit_SSD1306 display2(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, DISP_DC, DISP_RESET, DISP2_CS);
Adafruit_SSD1306 display3(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, DISP_DC, DISP_RESET, DISP3_CS);
Adafruit_SSD1306 display4(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, DISP_DC, DISP_RESET, DISP4_CS);
Adafruit_SSD1306 display5(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, DISP_DC, DISP_RESET, DISP5_CS);
Adafruit_SSD1306 display6(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, DISP_DC, DISP_RESET, DISP6_CS);
Adafruit_SSD1306 display7(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, DISP_DC, DISP_RESET, DISP7_CS);
Adafruit_SSD1306 display8(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, DISP_DC, DISP_RESET, DISP8_CS);
Adafruit_SSD1306 display9(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, DISP_DC, DISP_RESET, DISP9_CS);
Adafruit_SSD1306 display10(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, DISP_DC, DISP_RESET, DISP10_CS);
Adafruit_SSD1306 display11(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, DISP_DC, DISP_RESET, DISP11_CS);
Adafruit_SSD1306 display12(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, DISP_DC, DISP_RESET, DISP12_CS);
Adafruit_SSD1306 display13(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, DISP_DC, DISP_RESET, DISP13_CS);
Adafruit_SSD1306 display14(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, DISP_DC, DISP_RESET, DISP14_CS);
Adafruit_SSD1306 display15(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, DISP_DC, DISP_RESET, DISP15_CS);

Adafruit_PWMServoDriver pwm1 = Adafruit_PWMServoDriver(PWM1_ADRESS);
Adafruit_PWMServoDriver pwm2 = Adafruit_PWMServoDriver(PWM2_ADRESS);
Adafruit_PWMServoDriver pwm3 = Adafruit_PWMServoDriver(PWM3_ADRESS);

//Global variables 
/*float gVmsPump1Pressure = 7.8, gVmsPump2Pressure = 12.0, gVmsPressureRef = 7.0, gVmsMaxPressure = 15.0;
float gVmsTankWater = 5000.0, gVmsInflow = 0.0, gVmsOutflow = 0.0;
float gVmsPump2MaxInflow = 1600.0; //l/s
float gVmsTankMaxVol = 10000.0; //l
float gVmsPressureAct = 0.0; //Bar*/

vmsSimVarsStruct gVmsSimVars;

uint16_t gVmsPump1Speed = 566;
Valve gValve1(pwm2, RGB7, P0, P0, &pcf3, &pcf4), gValve2(pwm2, RGB10, P2, P1, &pcf3, &pcf4);
Pump gPump1(pwm2, RGB8), gPump2(pwm2, RGB9);

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

  pcfAllInInit(pcf3);
  pcfAllInInit(pcf4);
	
  pwmInit(pwm1);
  pwmInit(pwm2);
  pwmInit(pwm3);

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

  gPump1.pressure = 12.0;
  gPump1.maxInflow = 1600;
  gPump1.speed = 800;
  gPump2.pressure = 12.0;
  gPump2.maxInflow = 1600;

}
  

boolean test1 = 0;
uint16_t test2 = 0;
uint16_t test3 = 0;
uint16_t test4 = 0;
uint16_t progress = 0;
uint8_t state = 0;
uint32_t showLast = 0;

int task = 50; 

void loop()
{

  if(gValve1.valveState == Opened)
  {
    gValve1.open();
    gValve2.open();
  }
  
  if(gValve1.valveState == Closed)
  {
    gValve1.close();
    gValve2.close();
  }

  if(gValve1.valveState == 2)
  {
    gValve1.fail();
    gValve2.fail();
    Serial.println("Failure");
  }

  gValve1.readMode();
  gValve1.readState();


  Serial.print("State: ");
  Serial.println(gValve1.valveState);
  Serial.print("Mode: ");
  Serial.println(gValve1.valveMode);
  /*
  1. READ
  2. SIMULATE
  3. WRITE
  4. VISUALIZE
  */
  
  vmsSimluation(gPump1, gPump2, gValve1, gValve2, gVmsSimVars, task);

  vmsDispPump(display10, gPump1.speed, gPump1.pressure, gPump2.pressure);
  vmsDispPressure(display11, gVmsSimVars.PressureRef, gVmsSimVars.PressureAct);
  delay(task);

  /*RGBLedTest(5, pwm1);
  RGBLedTest(5, pwm2);
  RGBLedTest(4, pwm3);*/

  int analogData  = map(analogRead(20), 0, 8191, 0, 100);
  int analogData2  = map(analogRead(19), 0, 8191, 0, 100);
  //Serial.println(analogData);
  //Serial.println(analogData2);
  

  if(analogData2 < 20)
    gVmsSimVars.TankWater -= 4000;
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
  
}

