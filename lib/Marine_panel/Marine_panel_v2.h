#ifndef MARINE_PANEL_V2_H
#define MARINE_PANEL_V2_H

//PCF modules from different manufacturers may have different start up adresses!!! Needs to be verified by I2C scanner
#define PCF1_ADRESS 0x27 //0x38    
#define PCF2_ADRESS 0x21
#define PCF3_ADRESS 0x22
#define PCF4_ADRESS 0x23
#define PCF5_ADRESS 0x24
#define PCF6_ADRESS 0x25
#define PCF7_ADRESS 0x26
//#define PCF8_ADRESS 0x27


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
#define DISP_RESET  40

#define DISP_DC2    39
#define SPI_MOSI2   42
#define SPI_CLK2    41 

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
#define DISP14_CS   21
#define DISP15_CS   45

#define JOY1_X 20
#define JOY1_Y 19
#define JOY2_X 16
#define JOY2_Y 17

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




//installed libraries
#include <Wire.h>
#include <Arduino.h>
#include <PCF8574.h>
#include <stdbool.h>
#include <Adafruit_PWMServoDriver.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <RTClib.h>

//local libraries
#include "../Marine_panel/Fonts/DejavuSansMono15.h"
#include "../Marine_panel/Fonts/DejavuSansMono10.h"
#include "../Marine_panel/Fonts/DejavuSansMono8.h"
#include "Alarms.h"
#include "mbAdresses.h"
#include "../Marine_panel/Images/Images.h"



extern int task; 
extern int16_t alarmCounter;
extern int16_t alarmIndex;
extern bool newAlarmAdded;
extern bool alarmRemoved;
extern int16_t updatedAlarmRows;
extern int16_t updatedAlarmRows2;


enum mpMode{Local, Auto};
enum mpState{Closed, Opened, Failure, Failure2, Stopped, Starting, Stopping, StoppingF, Opening, Closing, Running, SlowingDown}; //states for valves and pumps 
enum class eRcsState{Local, Auto};



struct vmsSimVarsStruct
{
    float PressureRef, MaxPressure, PressureAct; //Bar
    float TankWater, TankMaxVol;                 //l
    float Inflow, Outflow;                       //l/s
};

struct busStruct
{
    float voltage, frequency, power; 
    bool live = false;
};



struct rcsVarsStruct
{
  float actRPM = 0.0, refRPM = 0.0, minRPM = -180.0, maxRPM = 180.0;
  float refPower = 0.0, actPower = 0.0, minPower = -2.0, maxPower = 4.0, nomPower = 3.6; //MW
  float refAngle = 0, actAngle = 0; 
  float refAngleSTBD = 0.0, refAnglePORT = 0.0, actAngleSTBD = 0.0, actAnglePORT = 0.0; 

  float refRpmBt = 0.0, actRpmBT = 0.0, minRpmBT = 0.0, maxRpmBT = 100.0;
  float refRpmStbdBT = 0.0, refRpmPortBT = 0.0, actRpmStbdBT = 0.0, actRpmPortBT = 0.0;
  float refPowerBT = 0.0, actPowerBT = 0.0, minPowerBT = 0.0, maxPowerBT = 1.0;
  float totPow = 0;
};


struct color
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

extern color Red;
extern color Green;
extern color Blue;
extern color None;

extern float pemsAvailiblePower;

struct alarmDispsStruct
{
  Adafruit_SSD1306 *d1;
  Adafruit_SSD1306 *d2;
  Adafruit_SSD1306 *d3;
  Adafruit_SSD1306 *d4;
};

enum fontAligment{LeftTop, RightTop, LeftBottom, RightBottom};

/*
  actState and prevState relates to state of button, true means button is pushed, false means its not pushed
  actValue is actual state of value connected to the button, actValue changes when button is pressed
*/
struct pushBtn
{
  bool actState, prevState;
  bool actValue;
  uint8_t pcfPin;
  uint16_t mbAddres;
};

struct twoStateBtn
{
  bool actValue;
  uint8_t pcfPin;
  uint16_t mbAddres;
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

    alarmDispsStruct *alarmDisps;
    mpAlarm valveAlarm1;
    uint16_t alarmRow = 0;

    RTC_DS1307 *rtc;

    bool openCmd = false;


    Valve(RTC_DS1307 *_rtc, alarmDispsStruct *_alarmDisps, mpAlarm _valveAlarm1, Adafruit_PWMServoDriver &_pwm, uint8_t _rgbNumber, uint8_t _pcf1Pin, uint8_t _pcf2Pin, PCF8574 *_pcf1 = NULL, PCF8574 *_pcf2 = NULL)
    {
      rgbNumber = _rgbNumber;
      pwm = _pwm;
      pcf1 = _pcf1;
      pcf2 = _pcf2;
      pcf1Pin = _pcf1Pin;
      pcf2Pin = _pcf2Pin;
      alarmDisps = _alarmDisps;
      valveAlarm1 = _valveAlarm1;
      rtc = _rtc;
      
    }
    
    void readMode();
    void readState(uint16_t mbAdr);
    void writeCmd();
    void writeMb(uint16_t mbAdrOpn, uint16_t mbAdrCls, uint16_t mbAdrFail, uint16_t mbAdrAut, uint16_t mbAdrLoc);
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
    float refSpeed, prevRefSpeed;
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

    alarmDispsStruct *alarmDisps;
    mpAlarm pumpAlarm1;
    uint16_t alarmRow = 0;

    RTC_DS1307 *rtc;

    bool run = false;
    uint16_t mbSpeedRef = 0;

    Pump(RTC_DS1307 *_rtc, alarmDispsStruct *_alarmDisps, mpAlarm _pumpAlarm1, Adafruit_PWMServoDriver &_pwm, uint8_t _rgbNumber, uint8_t _pcf1Pin, uint8_t _pcf2Pin, PCF8574 *_pcf1 = NULL, PCF8574 *_pcf2 = NULL)
    {
      rgbNumber = _rgbNumber;
      pwm = _pwm;
      pcf1 = _pcf1;
      pcf2 = _pcf2;
      pcf1Pin = _pcf1Pin;
      pcf2Pin = _pcf2Pin;

      alarmDisps = _alarmDisps;
      pumpAlarm1 = _pumpAlarm1;
      rtc = _rtc;
      //this->pumpAlarm1.time = rtcTime2String(*this->rtc);
    }
    
    
    void readMode();
    void readState(uint16_t mbAdr, uint16_t mbAdr2);
    void writeCmd();
    void writeMb(uint16_t mbAdrRun, uint16_t mbAdrStp, uint16_t mbAdrFail, uint16_t mbAdrAut, uint16_t mbAdrLoc, uint16_t mbPresAdr, bool vds = false);
    void savePrevState();
    void stopping(uint8_t loadTime, float dt, vmsSimVarsStruct &vmsSimVars);
    void starting(uint8_t loadTime, float dt, vmsSimVarsStruct &vmsSimVars);
    void slowingDown();
};





void incrementAlarmCounter(alarmDispsStruct &alarmDisps);
void decrementAlarmCounter(alarmDispsStruct &alarmDisps);
void printAlarmCounter();
void printAlarmIndex();
void resetAlarmIndex();

void RGBLedColor(uint8_t afirstPin, color aColor, Adafruit_PWMServoDriver pwm);
void RGBLedOff(uint8_t firstPin, Adafruit_PWMServoDriver pwm);
void RGBLedTest(uint8_t numOfLeds, Adafruit_PWMServoDriver &pwm);
void RGBLedBlink(Adafruit_PWMServoDriver &pwm, uint8_t firstPin, int durationOn, int durationOff, color aColor, unsigned long *timer);

void pcfAllOutInit(PCF8574 &pcf);
void pcfAllInInit(PCF8574 &pcf, uint16_t adress);
bool read2State(uint8_t pin, bool printOn, PCF8574 PCF8574);
uint8_t read3State(uint8_t firstPin, bool printOn, PCF8574 PCF8574);

void pwmInit(Adafruit_PWMServoDriver &pwm);

void dispInit(Adafruit_SSD1306 &display, bool reset);
void dispShowID(Adafruit_SSD1306 &display, String ID);
void dispShowAlarm(Adafruit_SSD1306 &display1, Adafruit_SSD1306 &display2, Adafruit_SSD1306 &display3, Adafruit_SSD1306 &display4, mpAlarm sAlarm, uint16_t row);
void dispClearAlarms(Adafruit_SSD1306 &display1, Adafruit_SSD1306 &display2, Adafruit_SSD1306 &display3, Adafruit_SSD1306 &display4);
void dispProgBarHorizontal(Adafruit_SSD1306 &display, int16_t x, uint8_t y, int16_t width, int16_t height, uint8_t progress);
void dispProgBarHorizontal2(Adafruit_SSD1306 &display, int16_t x, uint8_t y, int16_t width, int16_t height, int16_t progress, int16_t minVal, int16_t maxVal);
void dispProgBarVertical2(Adafruit_SSD1306 &display, int16_t x, uint8_t y, int16_t width, int16_t height, float progress, float minVal, float maxVal);
void dispStringALigned(String text, Adafruit_SSD1306 &display, GFXfont font, fontAligment aligment, int16_t x, int16_t y);

void vmsDispPump(Adafruit_SSD1306 &display, uint16_t speed, float pressure1, float pressure2);
void vmsDispPressure(Adafruit_SSD1306 &display, float pressure1, float pressure2);
void vmsSimluation(Pump &Pump1, Pump &Pump2, Valve &Valve1, Valve &Valve2, vmsSimVarsStruct &vmsSimVars, int task);
void vmsMbRead(vmsSimVarsStruct &aVmsSimVars);
void vmsMbWrite(vmsSimVarsStruct &aVmsSimVars);

void drawCirclePems(int16_t x0, int16_t y0, int16_t radius, Adafruit_SSD1306 &display, uint8_t progress, bool innerCircle = false);
void dispPemsVisualize(Adafruit_SSD1306 &display, uint8_t progress);

void rcsAzipodReadData(rcsVarsStruct &rcsVars, uint16_t task);
void rcsAzipodSimulate(rcsVarsStruct &rcsVars, busStruct bus, bool feederClosed);
void rcsBowThrustersReadData(rcsVarsStruct &rcsVars, uint16_t task);
void rcsBowThrustersSimulate(rcsVarsStruct &rcsVars, busStruct bus, bool feederClosed);
void dispRCSAzipodVisualize(Adafruit_SSD1306 &display, Adafruit_SSD1306 &display2, Adafruit_SSD1306 &display3, rcsVarsStruct &rcsVarsStruct);
void dispRCSBowThrustersVisualize(Adafruit_SSD1306 &display, Adafruit_SSD1306 &display2, Adafruit_SSD1306 &display3, rcsVarsStruct &rcsVars);
void dispDrawThrustBitmap(Adafruit_SSD1306& display, uint16_t thrustAngle);
void dispSimQuestion(Adafruit_SSD1306 &display);
void dispSimResult(Adafruit_SSD1306 &display, bool answer);

void rcsMbWrite(rcsVarsStruct &rcsVars);
void rcsMbRead(rcsVarsStruct &rcsVars);
uint8_t readPushBtn(pushBtn &btn, PCF8574 &pcf);
void writeTwoStateBtnMb(twoStateBtn &btn);
void writePushButMb(pushBtn &btn);


float addNoise(float value, float min, float max);
bool TOff(uint32_t delay, unsigned long *timer);

int joyReadData(uint8_t pin, bool verticalAxis = false);
void W5500Reset();

void rtcPrintTime(RTC_DS1307 &rtc);
String rtcTime2String(RTC_DS1307 &rtc);


#endif