#include "HVAC.h"
#include <Statistical.h>

bool gRegressionCalculated = false;
float gSlope, gOffset;



mbHvacSimVarsStruct mbHvacSimVars
{
  .pressureRef = 1600, .pressure = 1500, .pressMin = 0, .pressMax = 2000,      //Pa
  .tempRef = 21, .temp = 21, .tempMin = 18, .tempMax = 30,                     //°C
  .roomVolume = 10000, .airInRoom = 0                                              //m3
};


void Damper::readMode()
{
  uint8_t state = read3State(pcf1Pin, false, *pcf1);
  if(state == 0)
    this->damperMode = Local;

  if(state == 2)
    this->damperMode = Auto;
  
}

void Damper::readState(uint16_t mbAdr)
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
      this->openCmd = read2State(pcf2Pin, false, *pcf2);
    }
    else    //Auto - read from modbus
    {
      this->openCmd = arrayCoilsR[mbAdr];
    }

    bool closeCmd = !this->openCmd;

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
     
}

void Damper::writeCmd()
{
  uint32_t loadTime = 2000;
  //calculate first pin of pwm channel based on RGB number
  uint8_t firstPin = ((rgbNumber % 6) - 1) * 3;
  if(this->damperState == eDamperState::Failure)
  {
    RGBLedColor(firstPin, Red, pwm);
    dispShowAlarm(*this->alarmDisps->d1, *this->alarmDisps->d2, *this->alarmDisps->d3, *this->alarmDisps->d4, this->damperAlarm1, this->alarmRow);
  }
  else
  {
    if(this->damperState == eDamperState::Opened)
        RGBLedColor(firstPin, Blue, pwm);
    if(this->damperState == eDamperState::Closed)
        RGBLedColor(firstPin, None, pwm);
    if(this->damperState == eDamperState::Opening)
    {
      this->opening(loadTime);
      RGBLedBlink(pwm, firstPin, 500, 250, Blue, &this->blinkTimer);
    }
    if(this->damperState == eDamperState::Closing)
    {
      this->closing(loadTime);
      RGBLedBlink(pwm, firstPin, 500, 250, Blue, &this->blinkTimer);
    }
    if(this->damperState == eDamperState::ClosingF)
    {
      this->closing(loadTime);
      RGBLedBlink(pwm, firstPin, 500, 250, Red, &this->blinkTimer);
    }
      
  }

}

void Damper::writeMb(uint16_t mbAdrOpn, uint16_t mbAdrCls, uint16_t mbAdrFail, uint16_t mbAdrAut)
{
  bool fbOpn = false;
  bool fbCls = false;
  bool fbFail = false;
  bool fbAut = false;
  
  //Write feedbacks
  fbOpn = this->damperState == eDamperState::Opened;
  fbCls = this->damperState == eDamperState::Closed;
  if(this->damperMode == Auto)
    fbAut = true;

  bool damperFail = this->damperState == eDamperState::Failure|| this->damperState == eDamperState::failClogged;
  if(damperFail)
  {
    fbFail = true;
  }

  arrayCoilsW[mbAdrOpn - coilsWrOffset] = fbOpn;
  arrayCoilsW[mbAdrCls - coilsWrOffset] = fbCls;
  arrayCoilsW[mbAdrFail - coilsWrOffset] = fbFail;
  arrayCoilsW[mbAdrAut - coilsWrOffset] = fbAut;

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

void ValveLinear::readState(uint16_t mbAdr)
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
      this->openCmd = read2State(pcf2Pin, false, *pcf2);   
    }
    else    //Auto - read from modbus
    {
      this->openCmd = arrayCoilsR[mbAdr];
      this->openRef = uint16_t(arrayHregsR[Vlv3PosRef_ADR]/mbMultFactor);
      this->openAct = this->openRef;
    }

    bool closeCmd = !this->openCmd;

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

}


void ValveLinear::writeCmd()
{
  uint32_t loadTime = 2000;
  //calculate first pin of pwm channel based on RGB number
  uint8_t firstPin = ((rgbNumber % 6) - 1) * 3;
  if(this->valveState == eValveLinState::Failure)
  {
    RGBLedColor(firstPin, Red, pwm);
    dispShowAlarm(*this->alarmDisps->d1, *this->alarmDisps->d2, *this->alarmDisps->d3, *this->alarmDisps->d4, this->valveAlarm1, this->alarmRow);
  }
  else
  {
    if(this->valveState == eValveLinState::Opened)
        RGBLedColor(firstPin, Blue, pwm);
    if(this->valveState == eValveLinState::Closed)
        RGBLedColor(firstPin, None, pwm);
    if(this->valveState == eValveLinState::Opening)
    {
      this->opening(loadTime);
      RGBLedBlink(pwm, firstPin, 500, 250, Blue, &this->blinkTimer);
    }
    if(this->valveState == eValveLinState::Closing)
    {
      this->closing(loadTime);
      RGBLedBlink(pwm, firstPin, 500, 250, Blue, &this->blinkTimer);
    }
    if(this->valveState == eValveLinState::ClosingF)
    {
      this->closing(loadTime);
      RGBLedBlink(pwm, firstPin, 500, 250, Red, &this->blinkTimer);
    }
      
  }

}

void ValveLinear::writeMb(uint16_t mbAdrOpn, uint16_t mbAdrCls, uint16_t mbAdrFail, uint16_t mbAdrAut)
{
  bool fbOpn = false;
  bool fbCls = false;
  bool fbAut = false;
  bool fbFail = false;

  //Write feedbacks
  fbOpn = this->valveState == eValveLinState::Opened;
  fbCls = this->valveState == eValveLinState::Closed;
  if(this->valveMode == Auto)
    fbAut = true;

  bool valveFail = this->valveState == eValveLinState::Failure;

  if(valveFail)
  {
    fbFail = (this->valveState == eValveLinState::Failure);
  }

  arrayCoilsW[mbAdrOpn - coilsWrOffset] = fbOpn;
  arrayCoilsW[mbAdrCls - coilsWrOffset] = fbCls;
  arrayCoilsW[mbAdrFail - coilsWrOffset] = fbFail;
  arrayCoilsW[mbAdrAut - coilsWrOffset] = fbAut;

  arrayHregsW[Vlv3PosAct_ADR - HregsWrOffset] = uint16_t(this->openAct * mbMultFactor);
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

void Fan::readState(uint16_t mbAdr, uint16_t mbAdr2)
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
      this->run = read2State(pcf2Pin, false, *pcf2);
    }

    else    //Auto - read from modbus
    {
      this->run = arrayCoilsR[mbAdr];
      this->refSpeed = float(arrayHregsR[mbAdr2]/mbMultFactor);
    }

    bool stop = !this->run;
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
          RGBLedBlink(pwm, firstPin, 500, 250, Blue, &this->blinkTimer);
        }
        if(this->fanState == eFanState::Running)
        {
          RGBLedColor(firstPin, Blue, pwm);
          this->speed = addNoise(this->refSpeed, -5.0, 5.0);
        }
            //RGBLedColor(firstPin, 0, 255, 0, pwm);
        if(this->fanState == eFanState::Stopping)
        {
          this->stopping(loadTime);
          RGBLedBlink(pwm, firstPin, 500, 250, Blue, &this->blinkTimer);
        }
            
        if(this->fanState == eFanState::Stopped)
        {
          RGBLedColor(firstPin, None, pwm);
        }
            
        if(this->fanState == eFanState::StoppingF)
        {
          this->stopping(loadTime);
          RGBLedBlink(pwm, firstPin, 500, 250, Red, &this->blinkTimer);
        }
            
    }

      
}

void Fan::writeMb(uint16_t mbAdrRun, uint16_t mbAdrFail, uint16_t mbAdrAut)
{
  bool fbRun = false;
  bool fbAut = false;
  bool fbFail = false;

  uint16_t fbSpeed = 0;
  

  //Write feedbacks
  fbRun = this->fanState == eFanState::Running || this->fanState == eFanState::Starting || this->fanState == eFanState::Stopping || this->fanState == eFanState::StoppingF;
  fbSpeed = uint16_t(this->speed);
  if(this->fanMode == Auto)
    fbAut = true;

  bool fanFail = this->fanState == eFanState::Failure;

  if(fanFail)
  {
    fbFail = true;
  }

  arrayCoilsW[mbAdrRun - coilsWrOffset] = fbRun;
  arrayCoilsW[mbAdrFail - coilsWrOffset] = fbFail;
  arrayCoilsW[mbAdrAut - coilsWrOffset] = fbAut;

  arrayHregsW[FanPosAct_ADR - HregsWrOffset] = uint16_t(fbSpeed * mbMultFactor);
 
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

void hvacSimulation(Damper &damper1, Damper &damper2, ValveLinear &valve, Fan &fan, hvacSimVarsStruct &aHvacSimVars)
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
  float pressDiffPct = map(fan.maxStaticPressure - aHvacSimVars.pressure, 0, fan.maxStaticPressure, 0, 100);
  float pressInrease = map(fanSpeedPct, 0, 100, 0, maxPresIncrease) * pressDiffPct/100.0 * (task/1000.0);
  float maxPressDecrease = maxPresIncrease/10.0;
  float pressDecrease = maxPressDecrease * (1.0 - pressDiffPct/100.0) * (task/1000.0);


  //Active State
  if(damper1.damperState == eDamperState::Opened && damper2.damperState == eDamperState::Opened)
  {
    aHvacSimVars.pressure += fanSpeedPct/100.0 * pressInrease;
    aHvacSimVars.pressure = constrain(aHvacSimVars.pressure, aHvacSimVars.pressMin, fan.maxStaticPressure);

    //Cooling temperature
    if(valve.valveState == eValveLinState::Opened)
    {
      float airIncrease = fanSpeedPct/100.0 * pressInrease; //Lets say that airvolume increase is proportional to pressIncrease and fans rpm
      float tempDecreaseFactor = 0.2;
      float tempDecrease = airIncrease * 0.001;
      aHvacSimVars.temp -= tempDecrease;
      aHvacSimVars.temp = constrain(aHvacSimVars.temp, aHvacSimVars.tempMin, aHvacSimVars.tempMax);
    }
  }

  //Air leakage 
  aHvacSimVars.pressure -= pressDecrease;

  /*Debug
  Serial.println("Pressure incr: " + String((fanSpeedPct/100.0 * pressInrease)));
  Serial.println("Pressure decr: " + String(pressDecrease));*/

}

void hvacVisualization(Adafruit_SSD1306 &display, Fan &fan, hvacSimVarsStruct &aHvacSimVars)
{
  display.clearDisplay();
  
  String fanSpeedStr = "Fan speed: " + String(fan.speed, 0) + " rpm";
  String tempRefStr = "Temp. ref:  " + String(aHvacSimVars.tempRef, 1) + " C";
  String tempActStr = "Temp. act:  " + String(aHvacSimVars.temp, 1) + " C";
  String pressRefStr = "Press.ref: " + String(aHvacSimVars.pressureRef, 0) + " Pa";
  String pressActStr = "Press.act: " + String(aHvacSimVars.pressure, 0) + " Pa";

  uint8_t rowSpace = 13;

  dispStringALigned(fanSpeedStr, display, DejaVu_Sans_Mono_10, LeftTop, 0, 0);
  dispStringALigned(tempRefStr, display, DejaVu_Sans_Mono_10, LeftTop, 0, rowSpace);
  dispStringALigned(tempActStr, display, DejaVu_Sans_Mono_10, LeftTop, 0, 2 * rowSpace);
  dispStringALigned(pressRefStr, display, DejaVu_Sans_Mono_10, LeftTop, 0, 3 * rowSpace);
  dispStringALigned(pressActStr, display, DejaVu_Sans_Mono_10, LeftTop, 0, 4 * rowSpace);

  display.display();
}

void hvacWriteMb(hvacSimVarsStruct &aHvacSimVars)
{
  arrayHregsW[SnsrPressAct_ADR - HregsWrOffset] = uint16_t(aHvacSimVars.pressure * mbMultFactor);
  arrayHregsW[SnsrTempAct_ADR - HregsWrOffset] =  uint16_t(aHvacSimVars.temp * mbMultFactor);
}

void hvacReadMb(hvacSimVarsStruct &aHvacSimVars)
{
  aHvacSimVars.pressureRef = arrayHregsR[SnsrPressRef_ADR];
  aHvacSimVars.tempRef = arrayHregsR[SnsrTempRef_ADR];
}

