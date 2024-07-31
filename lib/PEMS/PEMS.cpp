#include "PEMS.h"




void Breaker::readMode()
{
  uint8_t state = read3State(pcf1Pin, false, *pcf1);
  if(state == 0)
    this->breakerMode = Local;

  if(state == 2)
    this->breakerMode = Auto;
}

void Breaker::readState(uint16_t cmdClsAdr, uint16_t cmdOpnAdr)
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
    if(this->breakerPrevState == eBreakerState::Closed || this->breakerState == eBreakerState::Closing)
    {
      this->timer = millis();
      this->breakerState = eBreakerState::OpeningF;
    }

    if(this->breakerPrevState == eBreakerState::Opened)    
    {
      this->breakerState = eBreakerState::Failure;
      incrementAlarmCounter(*this->alarmDisps);
      this->breakerAlarm1.time = rtcTime2String(*this->rtc);
      this->alarmRow = 1;
      if(alarmCounter > 1)
        newAlarmAdded = true;
      updatedAlarmRows = 0;
      dispClearAlarms(*this->alarmDisps->d1, *this->alarmDisps->d2, *this->alarmDisps->d3, *this->alarmDisps->d4);

    }
      
  }
    
        
  else
  {
    
    if(this->breakerMode == Local)
    {
        this->openCmd = read2State(pcf2Pin, false, *pcf2);     
        this->closeCmd = !openCmd;    
    }
    else    //Auto - read from modbus
    {
      if((arrayCoilsR[cmdClsAdr] && this->breakerState == eBreakerState::Opened))
      {
        this->openCmd = true;
        this->closeCmd = false;
      }

      if (arrayCoilsR[cmdOpnAdr] && this->breakerState == eBreakerState::Closed)
      {
        this->openCmd = false;
        this->closeCmd = true;
      }

        
    
    }

        if(openCmd && (this->breakerState == eBreakerState::Opened || this->breakerState == eBreakerState::Opening))
        {
          this->timer = millis(); //reset timer
          this->breakerState = eBreakerState::Closing;
        }
        if(closeCmd && (this->breakerPrevState == eBreakerState::Closed || this->breakerState == eBreakerState::Closing))
        {
          this->timer = millis(); //reset timer
          this->breakerState = eBreakerState::Opening;
        }
        if(closeCmd && (this->breakerPrevState == eBreakerState::Closed || this->breakerState == eBreakerState::Closing))
        {
          this->timer = millis(); //reset timer
          this->breakerState = eBreakerState::Opening;
        }
            
        if(closeCmd && (this->breakerPrevState == eBreakerState::Failure))
        {
          alarmIndex = this->alarmRow;
          if(alarmCounter > 1 && alarmIndex < alarmCounter)
          {
            alarmRemoved = true;
            updatedAlarmRows2 = alarmCounter - alarmIndex;
          }
            
          decrementAlarmCounter(*this->alarmDisps);
          this->alarmRow = 0;
          this->breakerState = eBreakerState::Opened;
        }
    
  }
}


void Breaker::writeCmd()
{
  uint32_t loadTime = 2000;
  //calculate first pin of pwm channel based on RGB number
  uint8_t firstPin = ((rgbNumber % 6) - 1) * 3;
  if(this->breakerState == eBreakerState::Failure)
  {
    if(newAlarmAdded)
      dispClearAlarms(*this->alarmDisps->d1, *this->alarmDisps->d2, *this->alarmDisps->d3, *this->alarmDisps->d4);
    dispShowAlarm(*this->alarmDisps->d1, *this->alarmDisps->d2, *this->alarmDisps->d3, *this->alarmDisps->d4, this->breakerAlarm1, this->alarmRow);
    RGBLedColor(firstPin, Red, pwm);
  }
  else
  {
    if(this->breakerState == eBreakerState::Closed)
        RGBLedColor(firstPin, Blue, pwm);
    if(this->breakerState == eBreakerState::Opened)
        RGBLedColor(firstPin, None, pwm);
    if(this->breakerState == eBreakerState::Closing)
    {
      this->opening(loadTime);
      RGBLedBlink(pwm, firstPin, 500, 250, Blue, &this->blinkTimer);
    }
    if(this->breakerState == eBreakerState::Opening)
    {
      this->closing(loadTime);
      RGBLedBlink(pwm, firstPin, 500, 250, Blue, &this->blinkTimer);
    }
    if(this->breakerState == eBreakerState::OpeningF)
    {
      this->closing(loadTime);
      RGBLedBlink(pwm, firstPin, 500, 250, Red, &this->blinkTimer);
    }
      
  }
  
  
}

void Breaker::writeMb(uint16_t mbAdrCls, uint16_t mbAdrOpn, uint16_t mbAdrFail, uint16_t mbAdrAut, uint16_t mbAdrLoc)
{
  //Local feeedback variables
  bool fbCls = false;
  bool fbOpn = false;
  bool fbAut = false;
  bool fbLoc = false;
  bool fbFail = false;

  //Write feedbacks
  fbOpn = this->breakerState == eBreakerState::Opened;
  fbCls = this->breakerState == eBreakerState::Closed;
  
  if(this->breakerMode == Auto)
    fbAut = true;
  else
    fbLoc = true;

  bool breakerFail = this->breakerState == eBreakerState::Failure;

  if(breakerFail)
    fbFail = true;

  arrayCoilsW[mbAdrCls - coilsWrOffset] = fbCls;
  arrayCoilsW[mbAdrOpn - coilsWrOffset] = fbOpn;
  arrayCoilsW[mbAdrFail - coilsWrOffset] = fbFail;
  arrayCoilsW[mbAdrAut - coilsWrOffset] = fbAut;
  arrayCoilsW[mbAdrLoc - coilsWrOffset] = fbLoc;
}

void Breaker::savePrevState()
{
    this->breakerPrevState = this->breakerState;
}


void Breaker::opening(uint32_t loadTime)
{
  if(TOff(loadTime, &this->timer))
  {
    this->breakerState = eBreakerState::Closed;
  }
    
    
}

void Breaker::closing(uint32_t loadTime)
{
  if(TOff(loadTime, &this->timer))
    this->breakerState = eBreakerState::Opened;
}

void Breaker::init()
{
  this->openCmd = true;
  this->closeCmd = false;
}

// ---------------------------- GENERATOR CLASS FUNCTIONS -------------------------------------------------
void Generator::readMode()
{
  uint8_t state = read3State(pcf1Pin, false, *pcf1);
  if(state == 0)
    this->generatorMode = Local;

  if(state == 2)
    this->generatorMode = Auto;
}

void Generator::readState(uint16_t startCmdAdr, uint16_t stopCmdAdr, uint16_t refPowAdr)
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
    if(this->generatorPrevState == eGeneratorState::StoppingF)
    {
      this->generatorState = eGeneratorState::Failure2;
      incrementAlarmCounter(*this->alarmDisps);
      this->generatorAlarm1.time = rtcTime2String(*this->rtc);
      this->alarmRow = 1;
      if(alarmCounter > 1)
        newAlarmAdded = true;
      updatedAlarmRows = 0;
      dispClearAlarms(*this->alarmDisps->d1, *this->alarmDisps->d2, *this->alarmDisps->d3, *this->alarmDisps->d4);
      
    }
    else if(this->generatorPrevState == eGeneratorState::Running || this->generatorPrevState == eGeneratorState::Starting)
        this->generatorState = eGeneratorState::StoppingF;
    else if(this->generatorPrevState == eGeneratorState::Stopped)   
    {
      this->generatorState = eGeneratorState::Failure;
      incrementAlarmCounter(*this->alarmDisps);
      this->generatorAlarm1.time = rtcTime2String(*this->rtc);
      this->alarmRow = 1;
      if(alarmCounter > 1)
        newAlarmAdded = true;
      updatedAlarmRows = 0;
      dispClearAlarms(*this->alarmDisps->d1, *this->alarmDisps->d2, *this->alarmDisps->d3, *this->alarmDisps->d4);
      
    }
        
  }
  else
  {
    if(this->generatorMode == Local)
    {
        this->run = read2State(pcf2Pin, false, *pcf2);
        this->stop = !run;
    }

    else    //Auto - read from modbus
    {

      //Read ref power
      //this->refPower = arrayHregsR[refPowAdr];
      
      //Start CMD rising edge
      if(arrayCoilsR[startCmdAdr] && !this->prevRunState)
      {
        this->run = true;
        this->stop = false;
      }

      //Stop CMD rising edge
      if(arrayCoilsR[stopCmdAdr] && this->prevRunState)
      {
        this->run = false;
        this->stop = true;
      }
       
      this->prevRunState = this->run;
    }

    

    if(run && (this->generatorPrevState == eGeneratorState::Stopped || this->generatorPrevState == eGeneratorState::Stopping))
      {
        this->timer = millis(); //reset timer
        this->generatorState = eGeneratorState::Starting;
      }
          
      if ((this->generatorPrevState == eGeneratorState::Running || this->generatorPrevState == eGeneratorState::Starting))
      {
        if(stop)
        {
          this->timer = millis(); //reset timer
          this->generatorState = eGeneratorState::Stopping;
        }
        else if(this->genBrkClosed)
        {
          this->generatorState = eGeneratorState::Delivering;
        }
          
      }

      if (this->generatorPrevState == eGeneratorState::Delivering)
      {
        if(stop)
        {
          this->timer = millis(); //reset timer
          this->generatorState = eGeneratorState::Stopping;
        }
        else if(!this->genBrkClosed)
        {
          this->generatorState = eGeneratorState::Running;
        }
          
      }

      if (stop && (this->generatorPrevState == eGeneratorState::Failure || this->generatorPrevState == eGeneratorState::Failure2))
      {
        alarmIndex = this->alarmRow;
        if(alarmCounter > 1 && alarmIndex < alarmCounter)
        {
          alarmRemoved = true;
          updatedAlarmRows2 = alarmCounter - alarmIndex;
        }
        decrementAlarmCounter(*this->alarmDisps);
        this->alarmRow = 0;
        this->generatorState = eGeneratorState::Stopped;
        
      }
    
  }

}

void Generator::writeCmd(rcsVarsStruct rcsVars, Generator aSecondGen, uint16_t incrAdr, uint16_t decrAdr, loadBankStruct loadBanks)
{   

    uint32_t loadTime = 2000;

    if(this->generatorState == eGeneratorState::Failure)
    {
      dispShowAlarm(*this->alarmDisps->d1, *this->alarmDisps->d2, *this->alarmDisps->d3, *this->alarmDisps->d4, this->generatorAlarm1, this->alarmRow);
      this->failure = false;
      this->dispState("Failure");
    }
    else if(this->generatorState == eGeneratorState::Failure2)
    {
      dispShowAlarm(*this->alarmDisps->d1, *this->alarmDisps->d2, *this->alarmDisps->d3, *this->alarmDisps->d4, this->generatorAlarm2, this->alarmRow);
      this->failure = false;
      this->dispState("Failure");
    }
      
        
    else
    {

      
        if(this->generatorState == eGeneratorState::Starting)
        {
          this->starting(loadTime);
          this->dispState("Starting");
        }
        if(this->generatorState == eGeneratorState::Running)
        {
          this->display->clearDisplay();
          this->dispState("Running");
          //values oscilating
          this->power = 0.0;
          this->speed = addNoise(this->nomSpeed, -5.0, 5.0);
          this->frequency = addNoise(this->nomFrequency, -0.5, 0.5);
          this->voltage = addNoise(this->nomVoltage, -5, 5);
        }

        if(this->generatorState == eGeneratorState::Delivering)
        {
          this->display->clearDisplay();
          this->dispState("Deliver.");

          float powerDeadband = 5.0;
          float droop = 0.05;

          //pulses from modbus
          bool incrPulse = arrayCoilsR[incrAdr];
          bool decrPulse = arrayCoilsR[decrAdr];
     
          //Load calculation
          this->loadPct = this->govIncrement/this->govIncrementMax;
               
          //when bustie breaker close and both delivering
          if(this->bustieClosed)
          {
            float numOfGens = 2.0;
            //both delivering
            if(aSecondGen.generatorState == eGeneratorState::Delivering)
            {
              //total power
              this->totalReqPow = ((rcsVars.actPower * 1000) + (rcsVars.actPowerBT * 1000)) + loadBanks.actPower1 + loadBanks.actPower2;;

              //Generator running without load or load step is present
              if(this->govIncrement == 0.0 || abs(this->totalReqPow - this->totalReqPowPrev) > 100.0) 
                this->reqPower = this->totalReqPow / numOfGens;
              else
                this->reqPower = this->totalReqPow - aSecondGen.power;

              //power incr/decr with governor pulses
              if(incrPulse)
              {
                this->reqPower += 10.0;
              }

              if(decrPulse)
              {
                this->reqPower -= 10.0;
              }
              this->totalReqPowPrev = this->totalReqPow;
            }
             
            //only one delivering
            else
              this->reqPower = ((rcsVars.actPower * 1000) + (rcsVars.actPowerBT * 1000)) + loadBanks.actPower1 + loadBanks.actPower2;
              
          }
          //Bus 1 and Bus 2 separated
          else
          {
            //Bus 1
            if(this->genId == 1)
              this->reqPower = (rcsVars.actPower * 1000) + loadBanks.actPower1;
            //Bus 2
            if(this->genId == 2)
             this->reqPower = (rcsVars.actPowerBT * 1000) + loadBanks.actPower2;
          }
          
          float powDiff = abs(this->reqPower - this->power);
          //Local mode - governor handling power requests
          if(this->generatorMode == Local)
          {
            if((this->power - powerDeadband) <= this->reqPower)
              this->power += powDiff * float(task)/1000.0 + powerDeadband/2;
            if((this->power + powerDeadband) >= this->reqPower)
             this->power -= powDiff * float(task)/1000.0 - + powerDeadband/2;
          }
        else
        {
          //power generation
          this->power = constrain(this->reqPower, this->minPower, this->maxPower);

          //Automatic increments when freq not ok
          if(this->frequency < this->nomFrequency)
        
          {
            this->govIncrement += 1.0;
            this->govIncrement = constrain(this->govIncrement, 0.0, this->nomSpeed * droop * 1.2);
          }

          if(this->frequency > this->nomFrequency)
          {
            this->govIncrement -= 1.0;
            this->govIncrement = constrain(this->govIncrement, 0.0, this->nomSpeed * droop * 1.2);
          }

          //voltage oscilating
          this->voltage = addNoise(this->nomVoltage, -1, 1);

          //speed and frequency calculation
          this->speed = (this->nomSpeed + this->govIncrement) - this->power * ((this->nomSpeed * droop) / this->nomPower);
          float poles = 8.0;
          this->frequency = (poles * this->speed)/120.0;
        }
          //Save prev requested power
          this->reqPowerPrev = this->reqPower;
        }
        if(this-> generatorState == eGeneratorState::Unloading)
        {
          this->display->clearDisplay();
          this->dispState("Unload.");
          this->unloading(8000);


        }
            //RGBLedColor(firstPin, 0, 255, 0, pwm);
        if(this->generatorState == eGeneratorState::Stopping)
        {
          this->stopping(loadTime);
          this->dispState("Stopping");
        }
            
        if(this->generatorState == eGeneratorState::Stopped)
        {
          this->dispState("Stopped");
        }
            
        if(this->generatorState == eGeneratorState::StoppingF)
        {
          this->stopping(loadTime);
          this->dispState("Stopp.F");
        }
            
    }

      
}


void Generator::savePrevState()
{
    this->generatorPrevState = this->generatorState;
}

void Generator::starting(uint32_t loadTime)
{   
 
 // this->power += (this->nomPower/loadTime) * task;
 // this->power = addNoise(this->power, -5.0, 5.0);

  this->speed += (this->nomSpeed/loadTime) * task;  
  this->speed = addNoise(this->speed, -5.0, 5.0);

  this->frequency += (this->nomFrequency/loadTime) * task;  
  this->frequency = addNoise(this->frequency, -0.5, 0.5);

  this->voltage += (this->nomVoltage/loadTime) * task;  
  this->voltage = addNoise(this->voltage, -0.5, 0.5);


  if(this->speed >= this->nomSpeed)
    this->generatorState = eGeneratorState::Running;
}

void Generator::stopping(uint32_t loadTime)
{

  this->speed -= (this->nomSpeed/loadTime) * task;
  this->speed = addNoise(this->speed, -5.0, 5.0);

  this->frequency -= (this->nomFrequency/loadTime) * task;  
  this->voltage -= (this->nomVoltage/loadTime) * task;  
  
  this->speed = constrain(this->speed, this->minSpeed, this->maxSpeed);
  this->frequency = constrain(this->frequency, 0, this->nomFrequency);
  this->voltage = constrain(this->voltage, 0, this->nomVoltage);

  if(this->speed <= this->minSpeed)
  {
    this->speed = this->minSpeed;
    this->power = this->minPower;
    if(this->generatorState == eGeneratorState::StoppingF)
      this->failure = true;
    else
      this->generatorState = eGeneratorState::Stopped;
  }
}

void Generator::unloading(uint32_t loadTime)
{
  this->power -= (this->nomPower/loadTime) * task;
  if(this->power <= this->minPower)
  {
    this->power = this->minPower;
    if(this->generatorState == eGeneratorState::StoppingF)
      this->failure = true;
    else
      this->generatorState = eGeneratorState::Running;
  }
}

void Generator::writeMb(uint16_t fbPowAdr, uint16_t fbRpmAdr, uint16_t fbVoltAdr, uint16_t fbFreqAdr, uint16_t fbAutAdr, uint16_t fbRun, uint16_t fbReady)
{
  arrayHregsW[fbPowAdr - HregsWrOffset] = uint16_t(this->power * mbMultFactor);
  arrayHregsW[fbRpmAdr - HregsWrOffset] = uint16_t(this->speed * mbMultFactor);
  arrayHregsW[fbVoltAdr - HregsWrOffset] = uint16_t(this->voltage * mbMultFactor);
  arrayHregsW[fbFreqAdr - HregsWrOffset] = uint16_t(this->frequency * mbMultFactor);

  arrayCoilsW[fbAutAdr - coilsWrOffset] = this->generatorMode == Auto;
  arrayCoilsW[fbRun - coilsWrOffset] = this->speed >= 100.0;
  arrayCoilsW[fbReady - coilsWrOffset] = this->generatorMode == Auto;
}

//IMPORTANT - function only clears display and write state into buffer. Text is displayed in visualize function.
void Generator::dispState(String text)  
{
  this->display->clearDisplay();
  this->display->setRotation(3);
  dispStringALigned(text, *this->display, DejaVu_Sans_Mono_10, LeftTop, 5, 0);
  this->display->setRotation(0);
}

void Generator::visualize()
{
  int bigRadius = 37;
  int smallRadius = 28;
  String powerStr = String(this->power, 0);
  String speedStr = String(this->speed, 0);

  int16_t cursorX = 0;
  int16_t cursorY = 40;

  //probably not needed vars
  uint8_t progressStringOffset = 15;
  int8_t physQtyOffset = -4;
  int8_t circleOffset = 5;



  uint8_t powerPct, speedPct;
  powerPct = map(this->power, this->minPower, this->maxPower, 0, 125);
  speedPct = map(this->speed, this->minSpeed, this->maxSpeed, 0, 100);


  drawCirclePems(DISP_CENTER_X0, DISP_CENTER_Y0 + circleOffset, bigRadius, *display, powerPct);
  drawCirclePems(DISP_CENTER_X0, DISP_CENTER_Y0 + circleOffset, bigRadius - 1, *display, powerPct, true);
  drawCirclePems(DISP_CENTER_X0, DISP_CENTER_Y0 + circleOffset, bigRadius - 2, *display, powerPct);

  drawCirclePems(DISP_CENTER_X0, DISP_CENTER_Y0 + circleOffset, smallRadius, *display, speedPct);
  drawCirclePems(DISP_CENTER_X0, DISP_CENTER_Y0 + circleOffset, smallRadius + 1 , *display, speedPct, true);
  drawCirclePems(DISP_CENTER_X0, DISP_CENTER_Y0 + circleOffset, smallRadius + 2, *display, speedPct);



  this->display->setFont(&DejaVu_Sans_Mono_10);
  this->display->setTextColor(SSD1306_WHITE);

  int16_t yOffset = 18;
  int16_t xOffsetPow = 75;
  int16_t xOffsetSpd = 75;

  if(this->power < 10.0)
    xOffsetPow = 66;
  else if (this->power >= 10.0 && this->power < 100.0)
    xOffsetPow = 69;
  else if (this->power >= 100.0 && this->power < 1000.0)
    xOffsetPow = 72;
  else
    xOffsetPow = 75;

  if(this->speed < 10.0)
    xOffsetSpd = 66;
  else if (this->speed >= 10.0 && this->speed < 100.0)
    xOffsetSpd = 69;
  else if (this->speed >= 100.0 && this->speed < 1000.0)
    xOffsetSpd = 72;
  else
    xOffsetSpd = 75;

  dispStringALigned(powerStr, *display, DejaVu_Sans_Mono_10, RightTop, xOffsetPow, yOffset);
  dispStringALigned("kw", *display, DejaVu_Sans_Mono_10, RightTop, 69, yOffset + 10);
  dispStringALigned(speedStr, *display, DejaVu_Sans_Mono_10, RightTop, xOffsetSpd, yOffset + 20);
  dispStringALigned("rpm", *display, DejaVu_Sans_Mono_10, RightTop, 72, yOffset + 30);

  
  //If generator started, refresh display less often
  int dispRefreshTime = 300;
  if(this->generatorState == eGeneratorState::Running && this->generatorPrevState == eGeneratorState::Running)
  {
    if(millis() - timer > dispRefreshTime)
    {
      this->display->display();
      timer = millis();
    }
  }
  else
    this->display->display();
}

void Generator::readGenBrkState(bool state1)
{
  this->genBrkClosed = state1;
}

void Generator::readBustieState(eBreakerState masterBr, eBreakerState slaveBr)
{
  this->bustieClosed = (masterBr == eBreakerState::Closed) && (slaveBr == eBreakerState::Closed);
}



void writeBusMb(busStruct bus1, busStruct bus2)
{
  //Voltage
  arrayHregsW[Bus1Volt_ADR - HregsWrOffset] = uint16_t(bus1.voltage * mbMultFactor);
  arrayHregsW[Bus2Volt_ADR - HregsWrOffset] =  uint16_t(bus2.voltage * mbMultFactor);

  //Frequency
  arrayHregsW[Bus1Freq_ADR - HregsWrOffset] = uint16_t(bus1.frequency * mbMultFactor);
  arrayHregsW[Bus2Freq_ADR - HregsWrOffset] =  uint16_t(bus2.frequency * mbMultFactor);
}


void readLoadBanksMb(loadBankStruct &loadBanks, busStruct bus1, busStruct bus2)
{
  //Load bank command
  loadBanks.refPower1 = float(arrayHregsR[LoadBankCmd1_ADR]/mbMultFactor);
  loadBanks.refPower2 = float(arrayHregsR[LoadBankCmd2_ADR]/mbMultFactor);

  //When bus live, loadbank consumes power
  if(bus1.live)
    loadBanks.actPower1 = loadBanks.refPower1;
  if(bus2.live)
    loadBanks.actPower2 = loadBanks.refPower2;
}

