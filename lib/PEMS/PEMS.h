#pragma once

#include "../Marine_panel/Marine_panel_v2.h"





enum class eBreakerState{Opened, Closed, Opening, Closing, Failure, OpeningF};

class Breaker
{
  public:
    eBreakerState breakerState = eBreakerState::Closed;
    eBreakerState breakerPrevState = eBreakerState::Closed;
    mpMode breakerMode = Local;
    uint8_t rgbNumber;
    Adafruit_PWMServoDriver pwm;

    PCF8574 *pcf1;
    PCF8574 *pcf2;
    uint8_t pcf1Pin;
    uint8_t pcf2Pin;

    unsigned long timer = 0;
    unsigned long blinkTimer = 0;
    unsigned long mbTimer = 0;

    bool mbRead = true;
    bool mbTask = false;
    bool mbOpenCmd = false;
    bool openCmd = false;

    alarmDispsStruct *alarmDisps;
    mpAlarm breakerAlarm1;
    uint16_t alarmRow = 0;

    RTC_DS1307 *rtc;

    Breaker(RTC_DS1307 *_rtc, alarmDispsStruct *_alarmDisps, mpAlarm _breakerAlarm1, Adafruit_PWMServoDriver &_pwm, uint8_t _rgbNumber, uint8_t _pcf1Pin, uint8_t _pcf2Pin, PCF8574 *_pcf1 = NULL, PCF8574 *_pcf2 = NULL)
    {
      rgbNumber = _rgbNumber;
      pwm = _pwm;
      pcf1 = _pcf1;
      pcf2 = _pcf2;
      pcf1Pin = _pcf1Pin;
      pcf2Pin = _pcf2Pin;

      alarmDisps = _alarmDisps;
      breakerAlarm1 = _breakerAlarm1;
      rtc = _rtc;
      //this->breakerAlarm1.time = rtcTime2String(*this->rtc);
    }
    
    void readMode();
    void readState(uint16_t mbAdr);
    void writeCmd();
    void writeMb(uint16_t mbAdrCls, uint16_t mbAdrOpn, uint16_t mbAdrFail, uint16_t mbAdrAut);
    void savePrevState();
    void opening(uint32_t loadTime);
    void closing(uint32_t loadTime);
    
};

class Generator
{
  public:
    float power = 0.0, nomPower = 2000.0 , minPower = 0.0, maxPower = 3000.0;
    float speed = 0.0, nomSpeed = 850.0, maxSpeed = 1500.0, minSpeed = 0.0;
    float voltage;
    float nomVoltage;
    float frequency;
    mpState generatorState = Stopped;
    mpState generatorPrevState = Stopped;
    mpMode generatorMode = Local;

    PCF8574 *pcf1;
    PCF8574 *pcf2;
    uint8_t pcf1Pin;
    uint8_t pcf2Pin;

    Adafruit_SSD1306 *display;

    unsigned long timer = 0;
    unsigned long mbTimer = 0;

    alarmDispsStruct *alarmDisps;
    mpAlarm generatorAlarm1;
    mpAlarm generatorAlarm2;
    uint16_t alarmRow = 0;

    bool breakersClosed = false;
    bool failure = false;

    bool mbRead = true;
    bool mbTask = false;
    bool run = false;

    RTC_DS1307 *rtc;

    Generator(RTC_DS1307 *_rtc, alarmDispsStruct *_alarmDisps, mpAlarm _generatorAlarm1, mpAlarm _generatorAlarm2, Adafruit_SSD1306 *_display, uint8_t _pcf1Pin, uint8_t _pcf2Pin, PCF8574 *_pcf1 = NULL, PCF8574 *_pcf2 = NULL)
    {
      pcf1 = _pcf1;
      pcf2 = _pcf2;
      pcf1Pin = _pcf1Pin;
      pcf2Pin = _pcf2Pin;
      display = _display;
      alarmDisps = _alarmDisps;
      generatorAlarm1 = _generatorAlarm1;
      generatorAlarm2 = _generatorAlarm2;
      rtc = _rtc;
    }
    
    
    void readMode();
    void readState(uint16_t mbAdr);
    void writeCmd();
    void writeMb(uint16_t fbPowAdr, uint16_t fbRpmAdr);
    void savePrevState();
    void stopping(uint32_t loadTime);
    void starting(uint32_t loadTime);
    void dispState(String text);
    void visualize();
    void readBreakersState(bool state1, bool state2);

};