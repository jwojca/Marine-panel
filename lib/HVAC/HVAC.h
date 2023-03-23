#pragma once

#include "../Marine_panel/Marine_panel_v2.h"

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
    void readState();
    void writeCmd();
    void savePrevState();
    void closing(uint32_t loadTime);
    void opening(uint32_t loadTime);
};



