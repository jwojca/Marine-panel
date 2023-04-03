#include "HVAC.h"
#include <Statistical.h>

bool gRegressionCalculated = false;
float gSlope, gOffset;

hvacSimVarsStruct hvacSimVars
{
  .pressureRef = 1600.0, .pressure = 1500.0, .pressMin = 0.0, .pressMax = 2000.0,      //Pa
  .tempRef = 21.0, .temp = 21.0, .tempMin = 18.0, .tempMax = 30.0,                     //°C
  .roomVolume = 10000.0, .airInRoom = 0.0                                              //m3
};


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
  if(alarmCounter == 0)
    alarmRemoved = false;

  if(newAlarmAdded && this->alarmRow > 0)
  {
    ++this->alarmRow;
    ++updatedAlarmRows;
    if(updatedAlarmRows == (alarmCounter - 1))
      newAlarmAdded = false;
    dispClearAlarms(*this->alarmDisps->d1, *this->alarmDisps->d2, *this->alarmDisps->d3, *this->alarmDisps->d4);
  }
    
  if(alarmRemoved && this->alarmRow > 0)
  {
    if(this->alarmRow > alarmIndex || this->alarmRow > alarmCounter)
    {
      --updatedAlarmRows2;
      --this->alarmRow;
      if(updatedAlarmRows2 == 0)
      {
        alarmRemoved = false;
        alarmIndex = 1000;
      }
        
      dispClearAlarms(*this->alarmDisps->d1, *this->alarmDisps->d2, *this->alarmDisps->d3, *this->alarmDisps->d4);
    }
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
      this->alarmRow = 1;
      if(alarmCounter > 1)
        newAlarmAdded = true;
      updatedAlarmRows = 0;
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
          alarmIndex = this->alarmRow;
          if(alarmCounter > 1 && alarmIndex < alarmCounter)
          {
            alarmRemoved = true;
            updatedAlarmRows2 = alarmCounter - alarmIndex;
          }
          decrementAlarmCounter(*this->alarmDisps);
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
  if(alarmCounter == 0)
    alarmRemoved = false;
    
  if(newAlarmAdded && this->alarmRow > 0)
  {
    ++this->alarmRow;
    ++updatedAlarmRows;
    if(updatedAlarmRows == (alarmCounter - 1))
      newAlarmAdded = false;
    dispClearAlarms(*this->alarmDisps->d1, *this->alarmDisps->d2, *this->alarmDisps->d3, *this->alarmDisps->d4);
  }
    
  if(alarmRemoved && this->alarmRow > 0)
  {
    if(this->alarmRow > alarmIndex || this->alarmRow > alarmCounter)
    {
      --updatedAlarmRows2;
      --this->alarmRow;
      if(updatedAlarmRows2 == 0)
      {
        alarmRemoved = false;
        alarmIndex = 1000;
      }
        
      dispClearAlarms(*this->alarmDisps->d1, *this->alarmDisps->d2, *this->alarmDisps->d3, *this->alarmDisps->d4);
    }
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
      this->alarmRow = 1;
      if(alarmCounter > 1)
        newAlarmAdded = true;
      updatedAlarmRows = 0;
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
          alarmIndex = this->alarmRow;
          if(alarmCounter > 1 && alarmIndex < alarmCounter)
          {
            alarmRemoved = true;
            updatedAlarmRows2 = alarmCounter - alarmIndex;
          }
          decrementAlarmCounter(*this->alarmDisps);
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

void Fan::readMode()
{
  uint8_t state = read3State(pcf1Pin, false, *pcf1);
  if(state == 0)
    this->fanMode = Local;

  if(state == 2)
    this->fanMode = Auto;
}

void Fan::readState()
{

   //For dynamic rows change
  if(alarmCounter == 0)
    alarmRemoved = false;
    
  if(newAlarmAdded && this->alarmRow > 0)
  {
    ++this->alarmRow;
    ++updatedAlarmRows;
    if(updatedAlarmRows == (alarmCounter - 1))
      newAlarmAdded = false;
    dispClearAlarms(*this->alarmDisps->d1, *this->alarmDisps->d2, *this->alarmDisps->d3, *this->alarmDisps->d4);
  }
    
  if(alarmRemoved && this->alarmRow > 0)
  {
    if(this->alarmRow > alarmIndex || this->alarmRow > alarmCounter)
    {
      --updatedAlarmRows2;
      --this->alarmRow;
      if(updatedAlarmRows2 == 0)
      {
        alarmRemoved = false;
        alarmIndex = 1000;
      }
        
      dispClearAlarms(*this->alarmDisps->d1, *this->alarmDisps->d2, *this->alarmDisps->d3, *this->alarmDisps->d4);
    }
  }
    

  //Read state
  uint8_t state = read3State(pcf1Pin, false, *pcf1);
  if(state == 1)
    this->failure = true;

  if(this->failure)
  {

    if(this->fanPrevState == eFanState::Running || this->fanPrevState == eFanState::Starting)
        this->fanState = eFanState::StoppingF;
    else if(this->fanPrevState == eFanState::Stopped)   
    {
      this->fanState = eFanState::Failure;
      incrementAlarmCounter(*this->alarmDisps);
      this->fanAlarm1.time = rtcTime2String(*this->rtc);
      this->alarmRow = 1;
      if(alarmCounter > 1)
        newAlarmAdded = true;
      updatedAlarmRows = 0;
    }
        
  }
  else
  {
    if(this->fanMode == Local)
    {
        bool run = read2State(pcf2Pin, false, *pcf2);
        bool stop = !run;

        if(run && (this->fanPrevState == eFanState::Stopped || this->fanPrevState == eFanState::Stopping))
        {
          this->timer = millis(); //reset timer
          this->fanState = eFanState::Starting;
        }
            
        if ((this->fanPrevState == eFanState::Running || this->fanPrevState == eFanState::Starting))
        {
          if(stop)
          {
            this->timer = millis(); //reset timer
            this->fanState = eFanState::Stopping;
          }
        }
        if (stop && (this->fanPrevState == eFanState::Failure))
        {
          alarmIndex = this->alarmRow;
          if(alarmCounter > 1 && alarmIndex < alarmCounter)
          {
            alarmRemoved = true;
            updatedAlarmRows2 = alarmCounter - alarmIndex;
          }
          decrementAlarmCounter(*this->alarmDisps);
          this->alarmRow = 0;
          this->fanState = eFanState::Stopped;
          
        }
            
    
    }

    else    //Auto - read from modbus
    {

    }
    
  }

}

void Fan::writeCmd()
{   

    uint32_t loadTime = 3000;
    uint8_t firstPin = ((rgbNumber % 6) - 1) * 3;
    if(this->fanState == eFanState::Failure)
    {
      dispShowAlarm(*this->alarmDisps->d1, *this->alarmDisps->d2, *this->alarmDisps->d3, *this->alarmDisps->d4, this->fanAlarm1, this->alarmRow);
      this->failure = false;
    }
     
    else
    {
        if(this->fanState == eFanState::Starting)
        {
          this->starting(loadTime);
          RGBLedBlink(pwm, firstPin, 500, 250, Green, &this->blinkTimer);
        }
        if(this->fanState == eFanState::Running)
        {
          RGBLedColor(firstPin, 0, 255, 0, pwm);
          this->speed = addNoise(this->refSpeed, -5.0, 5.0);
        }
            //RGBLedColor(firstPin, 0, 255, 0, pwm);
        if(this->fanState == eFanState::Stopping)
        {
          this->stopping(loadTime);
          RGBLedBlink(pwm, firstPin, 500, 250, Green, &this->blinkTimer);
        }
            
        if(this->fanState == eFanState::Stopped)
        {
          RGBLedColor(firstPin, 0, 0, 0, pwm);
        }
            
        if(this->fanState == eFanState::StoppingF)
        {
          this->stopping(loadTime);
          RGBLedBlink(pwm, firstPin, 500, 250, Red, &this->blinkTimer);
        }
            
    }

      
}


void Fan::savePrevState()
{
    this->fanPrevState = this->fanState;
}

void Fan::starting(uint32_t loadTime)
{   
  this->speed += (this->refSpeed/loadTime) * task;  
  this->speed = addNoise(this->speed, -5.0, 5.0);

  if(this->speed >= this->refSpeed)
    this->fanState = eFanState::Running;
}

void Fan::stopping(uint32_t loadTime)
{
  this->speed -= (this->refSpeed/loadTime) * task;
  this->speed = addNoise(this->speed, -5.0, 5.0);
  
  this->speed = constrain(this->speed, this->minSpeed, this->maxSpeed);

  if(this->speed <= this->minSpeed)
  {
    this->speed = this->minSpeed;
    this->fanState = eFanState::Stopped;
  }
}

void hvacSimulation(Damper &damper1, Damper &damper2, ValveLinear &valve, Fan &fan)
{
  
  if(!gRegressionCalculated)
  {
    float Data_X[] = {0.0, fan.maxAirFlow};
    float Data_Y[] = {fan.maxStaticPressure, 0.0};
    Linear_Regression<float, float> Regression(Data_X, Data_Y, sizeof(Data_X) / sizeof(Data_X[0]));
    gSlope = Regression.Slope();
    gOffset = Regression.Offset();

    Serial.print("Slope        : "); Serial.println(gSlope, 5);
    Serial.print("Offset       : "); Serial.println(gOffset, 5);
    gRegressionCalculated = true;
  }
  
  float fanSpeedPct = map(fan.speed, fan.minSpeed, fan.maxSpeed, 0, 100);
  float maxPresIncrease = 500.0; 
  float pressDiffPct = map(fan.maxStaticPressure - hvacSimVars.pressure, 0, fan.maxStaticPressure, 0, 100);
  float pressInrease = map(fanSpeedPct, 0, 100, 0, maxPresIncrease) * pressDiffPct/100.0 * (task/1000.0);
  float maxPressDecrease = maxPresIncrease/10.0;
  float pressDecrease = maxPressDecrease * (1.0 - pressDiffPct/100.0) * (task/1000.0);


  //Active State
  if(damper1.damperState == eDamperState::Opened && damper2.damperState == eDamperState::Opened)
  {
    hvacSimVars.pressure += fanSpeedPct/100.0 * pressInrease;
    hvacSimVars.pressure = constrain(hvacSimVars.pressure, hvacSimVars.pressMin, fan.maxStaticPressure);

    //Cooling temperature
    if(valve.valveState == eValveLinState::Opened)
    {
      float airIncrease = fanSpeedPct/100.0 * pressInrease; //Lets say that airvolume increase is proportional to pressIncrease and fans rpm
      float tempDecreaseFactor = 0.2;
      float tempDecrease = airIncrease * 0.001;
      hvacSimVars.temp -= tempDecrease;
      hvacSimVars.temp = constrain(hvacSimVars.temp, hvacSimVars.tempMin, hvacSimVars.tempMax);
    }
  }

  //Air leakage 
  hvacSimVars.pressure -= pressDecrease;

  /*Debug
  Serial.println("Pressure incr: " + String((fanSpeedPct/100.0 * pressInrease)));
  Serial.println("Pressure decr: " + String(pressDecrease));*/


}

void hvacVisualization(Adafruit_SSD1306 &display, Fan &fan)
{
  display.clearDisplay();
  
  String fanSpeedStr = "Fan speed: " + String(fan.speed, 0) + " rpm";
  String tempRefStr = "Temp. ref:  " + String(hvacSimVars.tempRef, 1) + " C";
  String tempActStr = "Temp. act:  " + String(hvacSimVars.temp, 1) + " C";
  String pressRefStr = "Press.ref: " + String(hvacSimVars.pressureRef, 0) + " Pa";
  String pressActStr = "Press.act: " + String(hvacSimVars.pressure, 0) + " Pa";

  uint8_t rowSpace = 13;

  dispStringALigned(fanSpeedStr, display, DejaVu_Sans_Mono_10, LeftTop, 0, 0);
  dispStringALigned(tempRefStr, display, DejaVu_Sans_Mono_10, LeftTop, 0, rowSpace);
  dispStringALigned(tempActStr, display, DejaVu_Sans_Mono_10, LeftTop, 0, 2 * rowSpace);
  dispStringALigned(pressRefStr, display, DejaVu_Sans_Mono_10, LeftTop, 0, 3 * rowSpace);
  dispStringALigned(pressActStr, display, DejaVu_Sans_Mono_10, LeftTop, 0, 4 * rowSpace);

  display.display();
}
