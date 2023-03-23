#include "HVAC.h"



void Damper::readMode()
{
  uint8_t state = read3State(pcf1Pin, false, *pcf1);
  if(state == 0)
    this->damperMode = Local;

  if(state == 2)
    this->damperMode = Auto;
  
}

void Damper::readState()
{

  //For dynamic rows change
  if(this->alarmRow > alarmIndex || this->alarmRow > alarmCounter)
  {
    --this->alarmRow;
    dispClearAlarms(*this->alarmDisps->d1, *this->alarmDisps->d2, *this->alarmDisps->d3, *this->alarmDisps->d4);
  }
    

  uint8_t state = read3State(pcf1Pin, false, *pcf1);
  if(state == 1)
  {
    if(this->damperPrevState == eDamperState::Opened || this->damperState == eDamperState::Opening)
    {
      this->timer = millis();
      this->damperState = eDamperState::ClosingF;
    }

    if(this->damperPrevState == eDamperState::Closed)   
    {
      this->damperState = eDamperState::Failure;
      this->damperAlarm1.time = rtcTime2String(*this->rtc);
      incrementAlarmCounter(*this->alarmDisps);
      this->alarmRow = alarmCounter;
    } 
      
  }
    
        
  else
  {
    if(this->damperMode == Local)
    {
        bool openCmd = read2State(pcf2Pin, false, *pcf2);
        bool closeCmd = !openCmd;

        if(openCmd && (this->damperState == eDamperState::Closed || this->damperState == eDamperState::Closing))
        {
          this->timer = millis(); //reset timer
          this->damperState = eDamperState::Opening;
        }
        if(closeCmd && (this->damperPrevState == eDamperState::Opened || this->damperState == eDamperState::Opening))
        {
          this->timer = millis(); //reset timer
          this->damperState = eDamperState::Closing;
        }
        if(closeCmd && (this->damperPrevState == eDamperState::Opened || this->damperState == eDamperState::Opening))
        {
          this->timer = millis(); //reset timer
          this->damperState = eDamperState::Closing;
        }
            
        if(closeCmd && (this->damperPrevState == eDamperState::Failure))
        {
          
          decrementAlarmCounter(*this->alarmDisps);
          alarmIndex = this->alarmRow;
          this->alarmRow = 0;
          this->damperState = eDamperState::Closed;
        }
          
    }
    else    //Auto - read from modbus
    {

    }
    
    
  }
     
}

void Damper::writeCmd()
{
  uint32_t loadTime = 2000;
  //calculate first pin of pwm channel based on RGB number
  uint8_t firstPin = ((rgbNumber % 6) - 1) * 3;
  if(this->damperState == eDamperState::Failure)
  {
    RGBLedColor(firstPin, 255, 0, 0, pwm);
    dispShowAlarm(*this->alarmDisps->d1, *this->alarmDisps->d2, *this->alarmDisps->d3, *this->alarmDisps->d4, this->damperAlarm1, this->alarmRow);
  }
  else
  {
    if(this->damperState == eDamperState::Opened)
        RGBLedColor(firstPin, 0, 255, 0, pwm);
    if(this->damperState == eDamperState::Closed)
        RGBLedColor(firstPin, 0, 0, 0, pwm);
    if(this->damperState == eDamperState::Opening)
    {
      this->opening(loadTime);
      RGBLedBlink(pwm, firstPin, 500, 250, Green, &this->blinkTimer);
    }
    if(this->damperState == eDamperState::Closing)
    {
      this->closing(loadTime);
      RGBLedBlink(pwm, firstPin, 500, 250, Green, &this->blinkTimer);
    }
    if(this->damperState == eDamperState::ClosingF)
    {
      this->closing(loadTime);
      RGBLedBlink(pwm, firstPin, 500, 250, Red, &this->blinkTimer);
    }
      
  }

}

void Damper::savePrevState()
{
    this->damperPrevState = this->damperState;
}

void Damper::opening(uint32_t loadTime)
{
  if(TOff(loadTime, &this->timer))
    this->damperState = eDamperState::Opened;
}

void Damper::closing(uint32_t loadTime)
{
  if(TOff(loadTime, &this->timer))
    this->damperState = eDamperState::Closed;
}


void ValveLinear::readMode()
{
  uint8_t state = read3State(pcf1Pin, false, *pcf1);
  if(state == 0)
    this->valveMode = Local;

  if(state == 2)
    this->valveMode = Auto;
  
}

void ValveLinear::readState()
{

  //For dynamic rows change
  if(this->alarmRow > alarmIndex || this->alarmRow > alarmCounter)
  {
    --this->alarmRow;
    dispClearAlarms(*this->alarmDisps->d1, *this->alarmDisps->d2, *this->alarmDisps->d3, *this->alarmDisps->d4);
  }
    

  uint8_t state = read3State(pcf1Pin, false, *pcf1);
  if(state == 1)
  {
    if(this->valvePrevState == eValveLinState::Opened || this->valveState == eValveLinState::Opening)
    {
      this->timer = millis();
      this->valveState = eValveLinState::ClosingF;
    }

    if(this->valvePrevState == eValveLinState::Closed)   
    {
      this->valveState = eValveLinState::Failure;
      this->valveAlarm1.time = rtcTime2String(*this->rtc);
      incrementAlarmCounter(*this->alarmDisps);
      this->alarmRow = alarmCounter;
    } 
      
  }
  
  else
  {
    if(this->valveMode == Local)
    {
        bool openCmd = read2State(pcf2Pin, false, *pcf2);
        bool closeCmd = !openCmd;

        if(openCmd && (this->valveState == eValveLinState::Closed || this->valveState == eValveLinState::Closing))
        {
          this->timer = millis(); //reset timer
          this->valveState = eValveLinState::Opening;
        }
        if(closeCmd && (this->valvePrevState == eValveLinState::Opened || this->valveState == eValveLinState::Opening))
        {
          this->timer = millis(); //reset timer
          this->valveState = eValveLinState::Closing;
        }
        if(closeCmd && (this->valvePrevState == eValveLinState::Opened || this->valveState == eValveLinState::Opening))
        {
          this->timer = millis(); //reset timer
          this->valveState = eValveLinState::Closing;
        }
            
        if(closeCmd && (this->valvePrevState == eValveLinState::Failure))
        {
          
          decrementAlarmCounter(*this->alarmDisps);
          alarmIndex = this->alarmRow;
          this->alarmRow = 0;
          this->valveState = eValveLinState::Closed;
        }
          
    }
    else    //Auto - read from modbus
    {

    }

  }

}


void ValveLinear::writeCmd()
{
  uint32_t loadTime = 2000;
  //calculate first pin of pwm channel based on RGB number
  uint8_t firstPin = ((rgbNumber % 6) - 1) * 3;
  if(this->valveState == eValveLinState::Failure)
  {
    RGBLedColor(firstPin, 255, 0, 0, pwm);
    dispShowAlarm(*this->alarmDisps->d1, *this->alarmDisps->d2, *this->alarmDisps->d3, *this->alarmDisps->d4, this->valveAlarm1, this->alarmRow);
  }
  else
  {
    if(this->valveState == eValveLinState::Opened)
        RGBLedColor(firstPin, 0, 255, 0, pwm);
    if(this->valveState == eValveLinState::Closed)
        RGBLedColor(firstPin, 0, 0, 0, pwm);
    if(this->valveState == eValveLinState::Opening)
    {
      this->opening(loadTime);
      RGBLedBlink(pwm, firstPin, 500, 250, Green, &this->blinkTimer);
    }
    if(this->valveState == eValveLinState::Closing)
    {
      this->closing(loadTime);
      RGBLedBlink(pwm, firstPin, 500, 250, Green, &this->blinkTimer);
    }
    if(this->valveState == eValveLinState::ClosingF)
    {
      this->closing(loadTime);
      RGBLedBlink(pwm, firstPin, 500, 250, Red, &this->blinkTimer);
    }
      
  }

}

void ValveLinear::savePrevState()
{
    this->valvePrevState = this->valveState;
}

void ValveLinear::opening(uint32_t loadTime)
{
  if(TOff(loadTime, &this->timer))
    this->valveState = eValveLinState::Opened;
}

void ValveLinear::closing(uint32_t loadTime)
{
  if(TOff(loadTime, &this->timer))
    this->valveState = eValveLinState::Closed;
}