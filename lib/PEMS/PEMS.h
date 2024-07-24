#pragma once

#include "../Marine_panel/Marine_panel_v2.h"

struct loadBankStruct
{
    float refPower1 = 0.0, actPower1 = 0.0; 
    float refPower2 = 0.0, actPower2 = 0.0;   
};


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
    bool closeCmd = false;

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
    void readState(uint16_t cmdClsAdr, uint16_t cmdOpnAdr);
    void writeCmd();
    void writeMb(uint16_t mbAdrCls, uint16_t mbAdrOpn, uint16_t mbAdrFail, uint16_t mbAdrAut, uint16_t mbAdrLoc);
    void savePrevState();
    void opening(uint32_t loadTime);
    void closing(uint32_t loadTime);
    
};

enum class eGeneratorState{Failure, Failure2, Stopped, Starting, Stopping, StoppingF, Opening, Closing, Running, Delivering, Unloading};

class Generator
{
  public:
    float power = 0.0, nomPower = 2500.0 , minPower = 0.0, maxPower = 3000.0, reqPower = 0.0, reqPowerPrev = 0.0,
                  reqPowerDroop = 0.0, totalReqPow = 0.0, totalReqPowPrev = 0.0;
    float speed = 0.0, nomSpeed = 900.0, maxSpeed = 1500.0, minSpeed = 0.0;
    float voltage = 0.0;
    float nomVoltage = 6000.0;
    float nomFrequency = 60.0;
    float frequency = 0.0;
    float govIncrement = 0.0, govIncrementMax = 54.0;
    float loadPct = 0.0;
    eGeneratorState generatorState = eGeneratorState::Stopped;
    eGeneratorState generatorPrevState = eGeneratorState::Stopped;
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
    uint16_t genId = 0;

    bool genBrkClosed = false;
    bool bustieClosed = false;
    bool failure = false;

    bool mbRead = true;
    bool mbTask = false;
    bool run = false;
    bool stop = false;
    bool prevRunState = false;

    RTC_DS1307 *rtc;

    Generator(RTC_DS1307 *_rtc, alarmDispsStruct *_alarmDisps, mpAlarm _generatorAlarm1, mpAlarm _generatorAlarm2, Adafruit_SSD1306 *_display, uint8_t _pcf1Pin, uint8_t _pcf2Pin, uint16_t _genId, PCF8574 *_pcf1 = NULL, PCF8574 *_pcf2 = NULL)
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
      genId = _genId;
    }
    
    
    void readMode();
    void readState(uint16_t startCmdAdr, uint16_t stopCmdAdr, uint16_t refPowAdr, uint16_t unloadAdr);
    void writeCmd(rcsVarsStruct rcsVars, Generator aSecondGen, uint16_t incrAdr, uint16_t decrAdr, loadBankStruct loadBanks);
    void writeMb(uint16_t fbPowAdr, uint16_t fbRpmAdr, uint16_t fbVoltAdr, uint16_t fbFreqAdr, uint16_t fbAutAdr, uint16_t fbRun, uint16_t fbReady);
    void savePrevState();
    void stopping(uint32_t loadTime);
    void starting(uint32_t loadTime);
    void unloading(uint32_t loadTime);
    void dispState(String text);
    void visualize();
    void readGenBrkState(bool state1);
    void readBustieState(eBreakerState masterBr, eBreakerState slaveBr);

};

void writeBusMb(busStruct bus1, busStruct bus2);
void readLoadBanksMb(loadBankStruct &loadBanks, busStruct bus1, busStruct bus2);
