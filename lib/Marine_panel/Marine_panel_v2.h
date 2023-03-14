#ifndef MARINE_PANEL_V2_H
#define MARINE_PANEL_V2_H

//PCF modules from different manufacturers may have different start up adresses!!! Needs to be verified by I2C scanner
#define PCF1_ADRESS 0x38    
#define PCF2_ADRESS 0x21
#define PCF3_ADRESS 0x22
#define PCF4_ADRESS 0x23
#define PCF5_ADRESS 0x24
#define PCF6_ADRESS 0x25
#define PCF7_ADRESS 0x26
#define PCF8_ADRESS 0x27

#define PWM1_ADRESS 0x40
#define PWM2_ADRESS 0x41
#define PWM3_ADRESS 0x42

#define DISP_CENTER_X0 64
#define DISP_CENTER_Y0 32

//Modbus
#define RST 0
#define PORT 502

// Declaration for SSD1306 display connected using software SPI (default case):
#define DISP_DC     33
#define DISP_RESET  44

#define DISP1_CS    1
#define DISP2_CS    2
#define DISP3_CS    3
#define DISP4_CS    4
#define DISP5_CS    5
#define DISP6_CS    6
#define DISP7_CS    7
#define DISP8_CS    10
#define DISP9_CS    11
#define DISP10_CS   12
#define DISP11_CS   13
#define DISP12_CS   14
#define DISP13_CS   15
#define DISP14_CS   16
#define DISP15_CS   45

#define RGB1 1
#define RGB2 2
#define RGB3 3
#define RGB4 4
#define RGB5 5
#define RGB6 1
#define RGB7 2
#define RGB8 3
#define RGB9 4
#define RGB10 5
#define RGB11 1
#define RGB12 2
#define RGB13 3
#define RGB14 4



#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels




#include <Wire.h>
#include <Arduino.h>
#include <PCF8574.h>
#include <stdbool.h>
#include <Adafruit_PWMServoDriver.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

//#include <SSD1306Spi.h>

#include "../Marine_panel/Fonts/DejavuSansMono15.h"
#include "../Marine_panel/Fonts/DejavuSansMono10.h"
#include "../Marine_panel/Fonts/DejavuSansMono8.h"
//#include "../lib/Marine_panel_VMS.h"

#include "../Marine_panel/Images/Images.h"


enum mpMode{Local, Auto};
enum mpState{Closed, Opened, Failure, Stopped, Starting, Stopping, StoppingF, Opening, Closing, Running}; //states for valves and pumps 
//TODO split states for valves, pumps, breakers, DGs


struct vmsSimVarsStruct
{
    float PressureRef = 7.0, MaxPressure = 15.0, PressureAct = 0.0; //Bar
    float TankWater = 5000.0, TankMaxVol = 10000.0;                 //l
    float Inflow = 0.0, Outflow = 0.0;                              //l/s
};

struct color
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};



class Valve
{
  public:
    mpState valveState = Closed;
    mpState valvePrevState = Closed;
    mpMode valveMode = Local;
    uint8_t rgbNumber;
    Adafruit_PWMServoDriver pwm;

    PCF8574 *pcf1;
    PCF8574 *pcf2;
    uint8_t pcf1Pin;
    uint8_t pcf2Pin;

    unsigned long timer = 0;
    unsigned long blinkTimer = 0;

    Valve(Adafruit_PWMServoDriver &_pwm, uint8_t _rgbNumber, uint8_t _pcf1Pin, uint8_t _pcf2Pin, PCF8574 *_pcf1 = NULL, PCF8574 *_pcf2 = NULL)
    {
      rgbNumber = _rgbNumber;
      pwm = _pwm;
      pcf1 = _pcf1;
      pcf2 = _pcf2;
      pcf1Pin = _pcf1Pin;
      pcf2Pin = _pcf2Pin;
    }
    
    void readMode();
    void readState();
    void writeCmd();
    void savePrevState();
    void closing(uint32_t loadTime);
    void opening(uint32_t loadTime);
};

class Pump
{
  public:
    float pressure;
    float nomPressure;
    float maxPressure;
    float speed;
    float maxSpeed;
    int maxInflow;
    int actInflow;
    mpState pumpState = Stopped;
    mpState pumpPrevState = Stopped;
    mpMode pumpMode = Local;
    uint8_t rgbNumber;
    Adafruit_PWMServoDriver pwm;

    PCF8574 *pcf1;
    PCF8574 *pcf2;
    uint8_t pcf1Pin;
    uint8_t pcf2Pin;

    unsigned long timer = 0;

    Pump(Adafruit_PWMServoDriver &_pwm, uint8_t _rgbNumber, uint8_t _pcf1Pin, uint8_t _pcf2Pin, PCF8574 *_pcf1 = NULL, PCF8574 *_pcf2 = NULL)
    {
      rgbNumber = _rgbNumber;
      pwm = _pwm;
      pcf1 = _pcf1;
      pcf2 = _pcf2;
      pcf1Pin = _pcf1Pin;
      pcf2Pin = _pcf2Pin;
    }
    
    
    void readMode();
    void readState();
    void writeCmd();
    void savePrevState();
    void stopping(uint8_t loadTime, float dt, vmsSimVarsStruct &vmsSimVars);
    void starting(uint8_t loadTime, float dt, vmsSimVarsStruct &vmsSimVars);

};

class Breaker
{
  public:
    mpState breakerState = Closed;
    mpState breakerPrevState = Closed;
    mpMode breakerMode = Local;
    uint8_t rgbNumber;
    Adafruit_PWMServoDriver pwm;

    PCF8574 *pcf1;
    PCF8574 *pcf2;
    uint8_t pcf1Pin;
    uint8_t pcf2Pin;

    unsigned long timer = 0;
    unsigned long blinkTimer = 0;

    Breaker(Adafruit_PWMServoDriver &_pwm, uint8_t _rgbNumber, uint8_t _pcf1Pin, uint8_t _pcf2Pin, PCF8574 *_pcf1 = NULL, PCF8574 *_pcf2 = NULL)
    {
      rgbNumber = _rgbNumber;
      pwm = _pwm;
      pcf1 = _pcf1;
      pcf2 = _pcf2;
      pcf1Pin = _pcf1Pin;
      pcf2Pin = _pcf2Pin;
    }
    
    void readMode();
    void readState();
    void writeCmd();
    void savePrevState();
    void closing(uint32_t loadTime);
    void opening(uint32_t loadTime);
};



void RGBLedColor(uint8_t afirstPin, uint8_t aRed, uint8_t aGreen, uint8_t aBlue, Adafruit_PWMServoDriver pwm);
void RGBLedOff(uint8_t firstPin, Adafruit_PWMServoDriver pwm);
void RGBLedTest(uint8_t numOfLeds, Adafruit_PWMServoDriver &pwm);
void RGBLedBlink(Adafruit_PWMServoDriver &pwm, uint8_t firstPin, int durationOn, int durationOff, color aColor, unsigned long timeNow);

void pcfAllOutInit(PCF8574 &pcf);
void pcfAllInInit(PCF8574 &pcf);
bool read2State(uint8_t pin, bool printOn, PCF8574 pcf8574);
uint8_t read3State(uint8_t firstPin, bool printOn, PCF8574 pcf8574);

void pwmInit(Adafruit_PWMServoDriver &pwm);

void dispInit(Adafruit_SSD1306 &display, bool reset);

void vmsDispPump(Adafruit_SSD1306 &display, uint16_t speed, float pressure1, float pressure2);
void vmsDispPressure(Adafruit_SSD1306 &display, float pressure1, float pressure2);
void vmsSimluation(Pump &Pump1, Pump &Pump2, Valve &Valve1, Valve &Valve2, vmsSimVarsStruct &vmsSimVars, int task);

void drawCirclePems(int16_t x0, int16_t y0, int16_t radius, Adafruit_SSD1306 &display, uint8_t progress, bool innerCircle = false);
void dispPemsVisualize(Adafruit_SSD1306 &display, uint8_t progress);

float addNoise(float value, float min, float max);

bool TOff(uint32_t delay, unsigned long *timer);

void W5500Reset();









/* MAIN COMMENT






void dispRCSAzipodVisualize(SSD1306Spi &display, SSD1306Spi &display2, SSD1306Spi &display3)
{
  //RPM
  uint8_t dispXOffset = 3;
  uint8_t dispYOffset = 10;
  display.clear();
  display.setFont(ArialMT_Plain_10);
  display.drawString(dispXOffset, dispYOffset-6, "RPM");
  display.setFont(ArialMT_Plain_10);
  display.drawString(dispXOffset, dispYOffset + 12, "100 port");
  display.drawString(dispXOffset + 58, dispYOffset + 12, "0");
  display.drawString(dispXOffset + 80, dispYOffset + 12, "stbd 100");
  display.drawString(dispXOffset + 55, dispYOffset - 5, "port");
  display.drawString(dispXOffset + 110, dispYOffset - 5, "%");
  display.setFont(ArialMT_Plain_16);
  display.drawString(dispXOffset + 78, dispYOffset - 10, "60.0");
  //display.drawString(dispXOffset + 83, dispYOffset - 5, "%");
  display.drawProgressBar(dispXOffset, dispYOffset + 27, 120, 20, 70);
  display.display();

  //Angle
  display2.clear();
  display2.drawXbm(0, 0, thrustWithcircle_width, thrustWithcircle_height, thrustWithcircle);
  display2.setFont(ArialMT_Plain_16);
  display2.drawString(0, 0, "150°");
  display2.setFont(ArialMT_Plain_10);
  display2.drawString(0, 15, "DEG actual");
  display2.setFont(ArialMT_Plain_16);
  display2.drawString(0, 37, "100°");
  display2.setFont(ArialMT_Plain_10);
  display2.drawString(0, 52, "DEG reference");
  display2.display();

  //Power
  uint8_t disp3XOffset = 3;
  uint8_t disp3YOffset = 10;
  display3.clear();
  display3.setFont(ArialMT_Plain_10);
  display3.drawString(disp3XOffset, disp3YOffset-6, "POWER");
  display3.setFont(ArialMT_Plain_10);
  display3.drawString(disp3XOffset + 2, disp3YOffset + 12, "0");
  display3.drawString(disp3XOffset + 105, disp3YOffset + 12, "1.1");
  display3.drawString(disp3XOffset + 103, disp3YOffset - 5, "MW");
  display3.setFont(ArialMT_Plain_16);
  display3.drawString(disp3XOffset + 78, disp3YOffset - 10, "0.2");
  display3.drawProgressBar(disp3XOffset, disp3YOffset + 27, 120, 20, 70);
  display3.display();
}

void dispAlarmVisualize(SSD1306Spi &display, SSD1306Spi &display2, SSD1306Spi &display3, SSD1306Spi &display4)
{
 
  //time and date
  uint8_t dispXOffset = 0;
  uint8_t dispYOffset = 20;
  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.drawString(dispXOffset, 0 * dispYOffset, "20-10-2022 08:49");
  display.drawString(dispXOffset, 1 * dispYOffset, "20-10-2022 08:48");
  display.drawString(dispXOffset, 2 * dispYOffset, "20-10-2022 08:47");
  display.display();

 
  display2.clear();
  display2.setFont(ArialMT_Plain_16);
  display2.drawString(dispXOffset, 0 * dispYOffset, "VMS Pump1");
  display2.drawString(dispXOffset, 1 * dispYOffset, "VMS Pump2");
  display2.drawString(dispXOffset, 2 * dispYOffset, "VMS Pump3");
  display2.display();

  display3.clear();
  display3.setFont(ArialMT_Plain_16);
  display3.drawString(dispXOffset, 0 * dispYOffset, "Power failure");
  display3.drawString(dispXOffset, 1 * dispYOffset, "Power failure");
  display3.drawString(dispXOffset, 2 * dispYOffset, "Power failure");
  display3.display();

  display4.clear();
  display4.setFont(ArialMT_Plain_16);
  display4.drawString(dispXOffset, 0 * dispYOffset, "Text Text Text");
  display4.drawString(dispXOffset, 1 * dispYOffset, "Text Text Text");
  display4.drawString(dispXOffset, 2 * dispYOffset, "Text Text Text");
  display4.display();
}

void mbTCPInit()
{
}

*/





#endif