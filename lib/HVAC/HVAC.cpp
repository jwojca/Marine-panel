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