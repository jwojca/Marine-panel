#pragma once

#include "../Marine_panel/Marine_panel_v2.h"

enum class eDamperState{Opened, Closed, Opening, Closing, Failure, ClosingF, failClogged};
enum class eValveLinState{Opened, Closed, Opening, Closing, Failure, ClosingF};
enum class eFanState{Stopped, Running, Starting, Stopping, Failure, StoppingF};

struct hvacSimVarsStruct
{
    float pressureRef, pressure, pressMin, pressMax;
    float tempRef, temp, tempMin, tempMax;
    float roomVolume;
    float airInRoom;
};

struct mbHvacSimVarsStruct
{
    uint16_t pressureRef, pressure, pressMin, pressMax;
    uint16_t tempRef, temp, tempMin, tempMax;
    uint16_t roomVolume;
    uint16_t airInRoom;
};

class Damper
{
  public:
    eDamperState damperState = eDamperState::Closed;
    eDamperState damperPrevState = eDamperState::Closed;
    mpMode damperMode = Local;
    uint8_t rgbNumber;
    Adafruit_PWMServoDriver pwm;

    PCF8574 *pcf1;
    PCF8574 *pcf2;
    uint8_t pcf1Pin;
    uint8_t pcf2Pin;

    unsigned long timer = 0;
    unsigned long blinkTimer = 0;

    alarmDispsStruct *alarmDisps;
    mpAlarm damperAlarm1;
    uint16_t alarmRow = 0;

    RTC_DS1307 *rtc;

    bool openCmd;

  Damper(RTC_DS1307 *_rtc, alarmDispsStruct *_alarmDisps, mpAlarm _damperAlarm1, Adafruit_PWMServoDriver &_pwm, uint8_t _rgbNumber, uint8_t _pcf1Pin, uint8_t _pcf2Pin, PCF8574 *_pcf1 = NULL, PCF8574 *_pcf2 = NULL)
    {
      rgbNumber = _rgbNumber;
      pwm = _pwm;
      pcf1 = _pcf1;
      pcf2 = _pcf2;
      pcf1Pin = _pcf1Pin;
      pcf2Pin = _pcf2Pin;
      alarmDisps = _alarmDisps;
      damperAlarm1 = _damperAlarm1;
      rtc = _rtc;
      
    }
    
    void readMode();
    void readState(uint16_t mbAdr);
    void writeCmd();
    void writeMb(uint16_t mbAdrOpn, uint16_t mbAdrCls, uint16_t mbAdrFail, uint16_t mbAdrAut);
    void savePrevState();
    void closing(uint32_t loadTime);
    void opening(uint32_t loadTime);
};

class ValveLinear
{
  public:
    eValveLinState valveState = eValveLinState::Closed;
    eValveLinState valvePrevState = eValveLinState::Closed;
    mpMode valveMode = Local;
    uint8_t rgbNumber;
    Adafruit_PWMServoDriver pwm;

    uint16_t openRef, openAct;
    bool openCmd;

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

    ValveLinear(RTC_DS1307 *_rtc, alarmDispsStruct *_alarmDisps, mpAlarm _valveAlarm1, Adafruit_PWMServoDriver &_pwm, uint8_t _rgbNumber, uint8_t _pcf1Pin, PCF8574 *_pcf1, uint8_t _pcf2Pin, PCF8574 *_pcf2)
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
    void writeMb(uint16_t mbAdrOpn, uint16_t mbAdrCls, uint16_t mbAdrFail, uint16_t mbAdrAut);
    void savePrevState();
    void closing(uint32_t loadTime);
    void opening(uint32_t loadTime);
};

class Fan
{
  public:
    float speed = 0.0, refSpeed = 540.0, maxSpeed = 1500.0, minSpeed = 0.0;     //rpm
    float actAirFlow = 0.0, maxAirFlow = 70000.0;        //m3/min
    float maxStaticPressure = 2800.0;                   //Pa

    eFanState fanState = eFanState::Stopped;
    eFanState fanPrevState = eFanState::Stopped;
    mpMode fanMode = Local;
    uint8_t rgbNumber;
    Adafruit_PWMServoDriver pwm;

    PCF8574 *pcf1;
    PCF8574 *pcf2;
    uint8_t pcf1Pin;
    uint8_t pcf2Pin;

    unsigned long timer = 0;
    unsigned long blinkTimer = 0;

    alarmDispsStruct *alarmDisps;
    mpAlarm fanAlarm1;
    mpAlarm fanAlarm2;
    uint16_t alarmRow = 0;

    bool breakersClosed = false;
    bool failure = false;
    bool run;

    

    RTC_DS1307 *rtc;

    Fan(RTC_DS1307 *_rtc, alarmDispsStruct *_alarmDisps, mpAlarm _fanAlarm1, Adafruit_PWMServoDriver &_pwm, uint8_t _rgbNumber, uint8_t _pcf1Pin, PCF8574 *_pcf1, uint8_t _pcf2Pin, PCF8574 *_pcf2)
    {
      rgbNumber = _rgbNumber;
      pwm = _pwm;
      pcf1 = _pcf1;
      pcf2 = _pcf2;
      pcf1Pin = _pcf1Pin;
      pcf2Pin = _pcf2Pin;
      alarmDisps = _alarmDisps;
      fanAlarm1 = _fanAlarm1;
      rtc = _rtc;
    }
    
    
    void readMode();
    void readState(uint16_t mbAdr, uint16_t mbAdr2);
    void writeCmd();
    void writeMb(uint16_t mbAdrRun, uint16_t mbAdrFail, uint16_t mbAdrAut);
    void savePrevState();
    void stopping(uint32_t loadTime);
    void starting(uint32_t loadTime);

};


void hvacVisualization(Adafruit_SSD1306 &display, Fan &fan, hvacSimVarsStruct &aHvacSimVars);
void hvacSimulation(Damper &damper1, Damper &damper2, ValveLinear &valve, Fan &fan, hvacSimVarsStruct &aHvacSimVars);
void hvacWriteMb(hvacSimVarsStruct &aHvacSimVars);
void hvacReadMb(hvacSimVarsStruct &aHvacSimVars);
