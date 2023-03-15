#include "Marine_panel_v2.h"



color Red{255, 0, 0};
color Green{0, 255, 0};
color Blue{0, 0, 255};

static int16_t alarmCounter = 0;

void incrementAlarmCounter(alarmDispsStruct &alarmDisps)
{
  
  ++alarmCounter;
  dispClearAlarms(*alarmDisps.d1, *alarmDisps.d2, *alarmDisps.d3, *alarmDisps.d4);
}

void decrementAlarmCounter(alarmDispsStruct &alarmDisps)
{
  --alarmCounter;
  if(alarmCounter < 0)
    alarmCounter = 0;
  dispClearAlarms(*alarmDisps.d1, *alarmDisps.d2, *alarmDisps.d3, *alarmDisps.d4);
}

void printAlarmCounter()
{
  Serial.println(alarmCounter);
}

void RGBLedColor(uint8_t afirstPin, uint8_t aRed, uint8_t aGreen, uint8_t aBlue, Adafruit_PWMServoDriver pwm)
{
  uint16_t red, green, blue;
  red = map(aRed, 0, 255, 0, 4095);
  green = map(aGreen, 0, 255, 0, 4095);
  blue = map(aBlue, 0, 255, 0, 4095);

  pwm.setPin(afirstPin, red);
  pwm.setPin(afirstPin + 1, green);
  pwm.setPin(afirstPin + 2, blue);
}


void RGBLedOff(uint8_t firstPin, Adafruit_PWMServoDriver pwm)
{
  pwm.setPin(firstPin, 0);
  pwm.setPin(firstPin + 1, 0);
  pwm.setPin(firstPin + 2, 0);
}

void RGBLedTest(uint8_t numOfLeds, Adafruit_PWMServoDriver &pwm)
{
  uint16_t delayTime = 500;
  
  for(uint8_t i = 0; i < numOfLeds*3; i += 3)
    RGBLedColor(i, 255, 0, 0, pwm);
  delay(delayTime);

  for(uint8_t i = 0; i < numOfLeds*3; i += 3)
    RGBLedColor(i, 0, 255, 0, pwm);
  delay(delayTime);

  for(uint8_t i = 0; i < numOfLeds*3; i += 3)
    RGBLedColor(i, 0, 0, 255, pwm);
  delay(delayTime);

}

void RGBLedBlink(Adafruit_PWMServoDriver &pwm, uint8_t firstPin, int durationOn, int durationOff, color aColor, unsigned long *timer)
{
    if(millis() - *timer > durationOn)
    {
        RGBLedColor(firstPin, 0, 0, 0, pwm);
    }
    if(millis() - *timer > durationOn + durationOff)
    {
        RGBLedColor(firstPin, aColor.red, aColor.green, aColor.blue, pwm);
        *timer = millis();
    }
}

bool TOff(uint32_t delay, unsigned long *timer)
{
   if(millis() - *timer > delay)
    {
        *timer = millis();
        return true;
    }
    else 
      return false;
}



void pcfAllOutInit(PCF8574 &pcf)
{
  // Set pinMode to OUTPUT
  pcf.pinMode(P0, OUTPUT);
  pcf.pinMode(P1, OUTPUT);
  pcf.pinMode(P2, OUTPUT);
  pcf.pinMode(P3, OUTPUT);
  pcf.pinMode(P4, OUTPUT);
  pcf.pinMode(P5, OUTPUT);
  pcf.pinMode(P6, OUTPUT);
  pcf.pinMode(P7, OUTPUT);

	Serial.print("Init pcf8574...");
	if (pcf.begin())
		Serial.println("OK");
  else
		Serial.println("KO");
}

void pcfAllInInit(PCF8574 &pcf)
{
  // Set pinMode to OUTPUT
  pcf.pinMode(P0, INPUT);
  pcf.pinMode(P1, INPUT);
  pcf.pinMode(P2, INPUT);
  pcf.pinMode(P3, INPUT);
  pcf.pinMode(P4, INPUT);
  pcf.pinMode(P5, INPUT);
  pcf.pinMode(P6, INPUT);
  pcf.pinMode(P7, INPUT);

	Serial.print("Init pcf8574...");
	if (pcf.begin())
		Serial.println("OK");
  else
		Serial.println("KO");
}

bool read2State(uint8_t pin, bool printOn, PCF8574 pcf8574)
{
  uint8_t val = pcf8574.digitalRead(pin);
  bool state;
  if(val == 0)
    state = true;
  else
    state = false;

  if (printOn)
  {
    Serial.print(state);
    Serial.print("\n"); 
  }  
 return state;
}

uint8_t read3State(uint8_t firstPin, bool printOn, PCF8574 pcf8574)
{
  uint8_t val1 = pcf8574.digitalRead(firstPin);
  uint8_t val2 = pcf8574.digitalRead(firstPin + 1);
  uint8_t state;
  if(val1 == 1 && val2 == 1)
    state = 0;
  else if(val2 == 0)
    state = 1;
  else
    state = 2;

   if (printOn)
  {
    Serial.print(state);
    Serial.print("\n"); 
  }  

  return state;
}




void pwmInit(Adafruit_PWMServoDriver &pwm)
{
  //PWM
  pwm.begin();
  pwm.setPWMFreq(50);
}

void W5500Reset()
{
  //W550 reset TODO is needed?
  pinMode(RST, OUTPUT);
  digitalWrite(RST, HIGH);
  delay(100);
  digitalWrite(RST, LOW);
  delay(100);
  digitalWrite(RST, HIGH);
  delay(100);
}



 void dispInit(Adafruit_SSD1306 &display, bool reset)
  {
    if(reset)
    {
      if(!display.begin(SSD1306_SWITCHCAPVCC, 0, true)) 
      {
        Serial.println(F("SSD1306 allocation failed"));
        for(;;); // Don't proceed, loop forever
      }
    }
    else
    {
      if(!display.begin(SSD1306_SWITCHCAPVCC, 0, false)) 
      {
        Serial.println(F("SSD1306 allocation failed"));
        for(;;); // Don't proceed, loop forever
      }
    }
    
  }

  void dispShowID(Adafruit_SSD1306 &display, String ID)
  {
    display.clearDisplay();
    display.setFont(&DejaVu_Sans_Mono_10);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 16);
    display.print(ID);
    display.display();
  }

  void dispDrawRow(Adafruit_SSD1306 &display, String val, uint16_t row)
  {
    int16_t rowIndex = 10 * row;
    display.setFont(&DejaVu_Sans_Mono_10);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, rowIndex);
    display.print(val);
    display.display();
  }

  void dispClearAlarms(Adafruit_SSD1306 &display1, Adafruit_SSD1306 &display2, Adafruit_SSD1306 &display3, Adafruit_SSD1306 &display4)
  {
    display1.clearDisplay();
    display2.clearDisplay();
    display3.clearDisplay();
    display4.clearDisplay();
    display1.display();
    display2.display();
    display3.display();
    display4.display();
  }


  //show alarm in one row
  void dispShowAlarm(Adafruit_SSD1306 &display1, Adafruit_SSD1306 &display2, Adafruit_SSD1306 &display3, Adafruit_SSD1306 &display4, mpAlarm sAlarm, uint16_t row)
  {
    if(row != 0)
    {
      dispDrawRow(display1, sAlarm.time, row);
      dispDrawRow(display2, sAlarm.objName, row);
      dispDrawRow(display3, sAlarm.eventKind, row);
      dispDrawRow(display4, sAlarm.descr, row);
    }
      
  }



void Valve::readMode()
{
  uint8_t state = read3State(pcf1Pin, false, *pcf1);
  if(state == 0)
    this->valveMode = Local;

  if(state == 2)
    this->valveMode = Auto;
  
}

void Valve::readState()
{

  //For dynamic rows change
  if(this->alarmRow > alarmCounter)
  {
    --this->alarmRow;
    dispClearAlarms(*this->alarmDisps->d1, *this->alarmDisps->d2, *this->alarmDisps->d3, *this->alarmDisps->d4);
  }
    

  uint8_t state = read3State(pcf1Pin, false, *pcf1);
  if(state == 1)
  {
    if(this->valvePrevState == Opened || this->valveState == Opening)
    {
      this->timer = millis();
      this->valveState = StoppingF;
    }

    if(this->valvePrevState == Closed)   
    {
      this->valveState = Failure;
      
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

        if(openCmd && (this->valveState == Closed || this->valveState == Closing))
        {
          this->timer = millis(); //reset timer
          this->valveState = Opening;
        }
        if(closeCmd && (this->valvePrevState == Opened || this->valveState == Opening))
        {
          this->timer = millis(); //reset timer
          this->valveState = Closing;
        }
        if(closeCmd && (this->valvePrevState == Opened || this->valveState == Opening))
        {
          this->timer = millis(); //reset timer
          this->valveState = Closing;
        }
            
        if(closeCmd && (this->valvePrevState == Failure))
        {
          
          decrementAlarmCounter(*this->alarmDisps);
          this->alarmRow = 0;
          this->valveState = Closed;
        }
          
    }
    else    //Auto - read from modbus
    {

    }
    
    
  }

  
      
}


void Valve::writeCmd()
{
  uint32_t loadTime = 2000;
  //calculate first pin of pwm channel based on RGB number
  uint8_t firstPin = ((rgbNumber % 6) - 1) * 3;
  if(this->valveState == Failure)
  {
    RGBLedColor(firstPin, 255, 0, 0, pwm);
    dispShowAlarm(*this->alarmDisps->d1, *this->alarmDisps->d2, *this->alarmDisps->d3, *this->alarmDisps->d4, this->valveAlarm1, this->alarmRow);
  }
  else
  {
    if(this->valveState == Opened)
        RGBLedColor(firstPin, 0, 255, 0, pwm);
    if(this->valveState == Closed)
        RGBLedColor(firstPin, 0, 0, 0, pwm);
    if(this->valveState == Opening)
    {
      this->opening(loadTime);
      RGBLedBlink(pwm, firstPin, 500, 250, Green, &this->blinkTimer);
    }
    if(this->valveState == Closing)
    {
      this->closing(loadTime);
      RGBLedBlink(pwm, firstPin, 500, 250, Green, &this->blinkTimer);
    }
    if(this->valveState == StoppingF)
    {
      this->closing(loadTime);
      RGBLedBlink(pwm, firstPin, 500, 250, Red, &this->blinkTimer);
    }
      
  }

}

void Valve::savePrevState()
{
    this->valvePrevState = this->valveState;
}

void Valve::opening(uint32_t loadTime)
{
  if(TOff(loadTime, &this->timer))
    this->valveState = Opened;
}

void Valve::closing(uint32_t loadTime)
{
  if(TOff(loadTime, &this->timer))
    this->valveState = Closed;
}

void Valve::decrementRow()
{
  --this->alarmRow;
  if(this->alarmRow <= 0)
    this->alarmRow = 0;
}

void Pump::readMode()
{
  uint8_t state = read3State(pcf1Pin, false, *pcf1);
  if(state == 0)
    this->pumpMode = Local;

  if(state == 2)
    this->pumpMode = Auto;
}

void Pump::readState()
{
  uint8_t state = read3State(pcf1Pin, false, *pcf1);
  if(state == 1)
  {
    if(this->pumpPrevState == Running || this->pumpPrevState == Starting)
        this->pumpState = StoppingF;
    if(this->pumpPrevState == Stopped)    
        this->pumpState = Failure;
  }
    
  else
  {
    if(this->pumpMode == Local)
    {
        bool run = read2State(pcf2Pin, false, *pcf2);
        bool stop = !run;

        if(run && (this->pumpPrevState == Stopped || this->pumpPrevState == Stopping))
            this->pumpState = Starting;
        if (stop && (this->pumpPrevState == Running || this->pumpPrevState == Starting))
            this->pumpState = Stopping;
        if (stop && (this->pumpPrevState == Failure))
            this->pumpState = Stopped;
    
    }
    else    //Auto - read from modbus
    {

    }
    
  }

}

void Pump::writeCmd()
{   
    int rgbBlinkDelay = 500;
    //calculate first pin of pwm channel based on RGB number
    uint8_t firstPin = ((rgbNumber % 6) - 1) * 3;
    if(this->pumpState == Failure)
        RGBLedColor(firstPin, 255, 0, 0, pwm);
    else
    {
        if(this->pumpState == Starting)
            RGBLedBlink(pwm, firstPin, 500, 250, Green, &this->timer);
        if(this->pumpState == Running)
            RGBLedColor(firstPin, 0, 255, 0, pwm);
        if(this->pumpState == Stopping)
            RGBLedBlink(pwm, firstPin, 500, 250, Green, &this->timer);
        if(this->pumpState == Stopped)
            RGBLedColor(firstPin, 0, 0, 0, pwm);
        if(this->pumpState == StoppingF)
            RGBLedBlink(pwm, firstPin, 500, 250, Red, &this->timer);
    }
  
}

void Pump::savePrevState()
{
    this->pumpPrevState = this->pumpState;
}

void Pump::starting(uint8_t loadTime, float dt, vmsSimVarsStruct &vmsSimVars)
{

    this->pressure += (this->nomPressure/loadTime) * dt;
    this->pressure = addNoise(this->pressure, -0.15, 0.15);

    float dp = (this->pressure - vmsSimVars.PressureAct) * 0.1;
    this->actInflow = this->maxInflow * dp;  

    this->speed += (this->maxSpeed/loadTime) * dt;;

    if(this->pressure > this->nomPressure)
        this->pumpState = Running;
}

void Pump::stopping(uint8_t loadTime, float dt, vmsSimVarsStruct &vmsSimVars)
{

    this->pressure -= (this->nomPressure/loadTime) * dt;
    this->pressure = addNoise(this->pressure, -0.15, 0.15);

    //float dp = (this->pressure - vmsSimVars.PressureAct) * 0.1;
    //this->actInflow = this->maxInflow * dp;  

    this->speed -= (this->maxSpeed/loadTime) * dt;

    this->speed = constrain(this->speed, 0, this->maxSpeed);
    this->pressure = constrain(this->pressure, 0, maxPressure);

    if(this->speed <= 0 || this->pressure <= 0)
        this->pumpState = Stopped;
}

void Breaker::readMode()
{
  uint8_t state = read3State(pcf1Pin, false, *pcf1);
  if(state == 0)
    this->breakerMode = Local;

  if(state == 2)
    this->breakerMode = Auto;
}

void Breaker::readState()
{


  

  uint8_t state = read3State(pcf1Pin, false, *pcf1);
  if(state == 1)
  {
    if(this->breakerPrevState == Opened || this->breakerState == Opening)
    {
      this->timer = millis();
      this->breakerState = StoppingF;
    }

    if(this->breakerPrevState == Closed)    
      this->breakerState = Failure;
  }
    
        
  else
  {
    if(this->breakerMode == Local)
    {
        bool openCmd = read2State(pcf2Pin, false, *pcf2);
        bool closeCmd = !openCmd;

        if(openCmd && (this->breakerState == Closed || this->breakerState == Closing))
        {
          this->timer = millis(); //reset timer
          this->breakerState = Opening;
        }
        if(closeCmd && (this->breakerPrevState == Opened || this->breakerState == Opening))
        {
          this->timer = millis(); //reset timer
          this->breakerState = Closing;
        }
        if(closeCmd && (this->breakerPrevState == Opened || this->breakerState == Opening))
        {
          this->timer = millis(); //reset timer
          this->breakerState = Closing;
        }
            
        if(closeCmd && (this->breakerPrevState == Failure))
          this->breakerState = Closed;
    }
    else    //Auto - read from modbus
    {

    }
    
  }
}


void Breaker::writeCmd()
{
  uint32_t loadTime = 2000;
  //calculate first pin of pwm channel based on RGB number
  uint8_t firstPin = ((rgbNumber % 6) - 1) * 3;
  if(this->breakerState == Failure)
    RGBLedColor(firstPin, 255, 0, 0, pwm);
  else
  {
    if(this->breakerState == Opened)
        RGBLedColor(firstPin, 0, 255, 0, pwm);
    if(this->breakerState == Closed)
        RGBLedColor(firstPin, 0, 0, 0, pwm);
    if(this->breakerState == Opening)
    {
      this->opening(loadTime);
      RGBLedBlink(pwm, firstPin, 500, 250, Green, &this->blinkTimer);
    }
    if(this->breakerState == Closing)
    {
      this->closing(loadTime);
      RGBLedBlink(pwm, firstPin, 500, 250, Green, &this->blinkTimer);
    }
    if(this->breakerState == StoppingF)
    {
      this->closing(loadTime);
      RGBLedBlink(pwm, firstPin, 500, 250, Red, &this->blinkTimer);
    }
      
  }
  
  
}

void Breaker::savePrevState()
{
    this->breakerPrevState = this->breakerState;
}

void Breaker::opening(uint32_t loadTime)
{
  if(TOff(loadTime, &this->timer))
    this->breakerState = Opened;
}

void Breaker::closing(uint32_t loadTime)
{
  if(TOff(loadTime, &this->timer))
    this->breakerState = Closed;
}

void vmsDispPump(Adafruit_SSD1306 &display, uint16_t speed, float pressure1, float pressure2)
{
  display.clearDisplay();
  display.setFont(&DejaVu_Sans_Mono_15);
  display.setTextColor(SSD1306_WHITE);

  int16_t cursorX = 0;
  int16_t cursorY = 17;

  display.setCursor(cursorX, cursorY);
  display.println(F("P1:      RPM"));
  display.println(F("P1:      Bar"));
  display.println(F("P2:      Bar"));

  int16_t cursorOffsetX = 55;
  display.setCursor(cursorX + cursorOffsetX, cursorY);
  display.println(speed);

  cursorY = display.getCursorY();
  if(pressure1 >= 10.0)
    cursorOffsetX = 45;
  else  
    cursorOffsetX = 55;
  display.setCursor(cursorX + cursorOffsetX, cursorY);
  display.printf("%.1f",pressure1);
  display.println();

  cursorY = display.getCursorY();
  if(pressure2 >= 10.0)
    cursorOffsetX = 45;
  else  
    cursorOffsetX = 55;
  display.setCursor(cursorX + cursorOffsetX, cursorY);
  display.printf("%.1f",pressure2);

  display.display();
}

void vmsDispPressure(Adafruit_SSD1306 &display, float pressure1, float pressure2)
{
  display.clearDisplay();
  display.setFont(&DejaVu_Sans_Mono_15);
  display.setTextColor(SSD1306_WHITE);

  int16_t cursorX = 0;
  int16_t cursorY = 17;

  display.setCursor(cursorX, cursorY);
  display.println(F("  Pressure  "));
  display.println(F("Ref:     Bar"));
  display.println(F("Act:     Bar"));

  display.setCursor(cursorX, cursorY);
  display.println();

  int16_t cursorOffsetX = 55;
  cursorY = display.getCursorY();
  display.setCursor(cursorX + cursorOffsetX, cursorY);
  display.printf("%.1f",pressure1);
  display.println();

  cursorY = display.getCursorY();
  display.setCursor(cursorX + cursorOffsetX, cursorY);
  display.printf("%.1f",pressure2);

  display.display();
}

void vmsSimluation(Pump &Pump1, Pump &Pump2, Valve &Valve1, Valve &Valve2, vmsSimVarsStruct &vmsSimVars, int task)
{
    float dt = (float)task/1000;

    if(Pump1.pumpState == Starting)
    {
        Pump1.starting(3, dt, vmsSimVars);
    }
    else if(Pump1.pumpState == Running)
    {
        Pump1.pressure = Pump1.nomPressure;
        Pump1.pressure = addNoise(Pump1.pressure, -0.15, 0.15);
        float dp = (Pump1.pressure - vmsSimVars.PressureAct) * 0.1;
        Pump1.actInflow = Pump1.maxInflow * dp;  //TODO how to handle valveState multiplication??
        Pump1.speed = 955;
        Pump1.speed += random(-10, 10);
    }
    else if(Pump1.pumpState == Stopping || Pump1.pumpState == StoppingF)
        Pump1.stopping(3, dt, vmsSimVars);
    else
    {
        Pump1.pressure = 0;
        Pump1.actInflow = 0;
        Pump1.speed = 0;
    }



    if(Pump2.pumpState == Starting)
        Pump2.starting(3, dt, vmsSimVars);
    else if(Pump2.pumpState == Running)
    {   
        Pump2.pressure = Pump2.nomPressure;
        Pump2.pressure = addNoise(Pump2.pressure, -0.15, 0.15);
        float dp = (Pump2.pressure - vmsSimVars.PressureAct) * 0.1;
        Pump2.actInflow = Pump2.maxInflow * dp;
        Pump2.speed = 955;
        Pump2.speed += random(-10, 10);
    }
    else if(Pump2.pumpState == Stopping || Pump2.pumpState == StoppingF)
        Pump2.stopping(3, dt, vmsSimVars);
    else
    {
        Pump2.pressure = 0;
        Pump2.actInflow = 0;
        Pump2.speed = 0;
    }

    //If in these states there is inflow/outflow. 
    float v1On = float(Valve1.valveState == Opened || Valve1.valveState == Opening || Valve1.valveState == Closing);
    float v2On = float(Valve2.valveState == Opened || Valve2.valveState == Opening || Valve2.valveState == Closing);

    vmsSimVars.Inflow = dt * v1On * (Pump1.actInflow + Pump2.actInflow);

    //TODO define v2 outflow
    vmsSimVars.Outflow = dt * v2On * 100;



    //quick outflow of water, when V2 opens suddenly
    if(Valve2.valveState == Opening && Valve2.valvePrevState == Closed)
        vmsSimVars.TankWater -= vmsSimVars.PressureAct * 100;
 
    vmsSimVars.TankWater = vmsSimVars.TankWater + vmsSimVars.Inflow - vmsSimVars.Outflow;
    vmsSimVars.TankWater = constrain(vmsSimVars.TankWater, 0, vmsSimVars.TankMaxVol);
    //Serial.println(vmsSimVars.TankWater);

    //pressure equation
    vmsSimVars.PressureAct = (vmsSimVars.MaxPressure * 1000) / (vmsSimVars.TankMaxVol - vmsSimVars.TankWater);

}

float addNoise(float value, float min, float max)
{
    float randf = (float)random(0, 1000)/1000; //random 0-1 float with 10^-3 precision
    value = value + min + (max - min)*randf;
    return value;
}

void drawCirclePems(int16_t x0, int16_t y0, int16_t radius, Adafruit_SSD1306 &display, uint8_t progress, bool innerCircle) 
{
  int16_t x = 0, y = radius;
	int16_t dp = 1 - radius;
  uint16_t totalCirclePixels = 0;
  uint16_t pixelPerOctant = 0;
  uint16_t progressPixels = 0;
  uint16_t numOfActivePixels = 0;

  //get number of total Pemscircle pixels
  do 
  {
    if (dp < 0)
      dp = dp + (x++) * 2 + 3;
    else
      dp = dp + (x++) * 2 - (y--) * 2 + 5;
    ++pixelPerOctant;
  } while (x < y);

  totalCirclePixels = pixelPerOctant*6 + 3;
  progressPixels = map(progress, 0, 100, 0, totalCirclePixels);

  //create array for x,y data
  int16_t array[pixelPerOctant][2];
  uint16_t arrIndex = pixelPerOctant-1;

  //reset params
  x = 0;
  y = radius;
  dp = 1 - radius;

  //load x,y data into array
   do 
  {
    if (dp < 0)
      dp = dp + (x++) * 2 + 3;
    else
      dp = dp + (x++) * 2 - (y--) * 2 + 5;
    array[arrIndex][0] = x;
    array[arrIndex][1] = y;
    --arrIndex;
  } while (x < y);


  //main for cycle for 6 octants
  for(uint8_t i = 0; i < 6; ++i)
  {
    x = 0;
    y = radius;
    dp = 1 - radius;
    uint8_t indexIncrement = 0;
    uint8_t indexDecrement = pixelPerOctant-1;

    if(numOfActivePixels >= progressPixels)
        break;
    //loop for draw individual pixels, odd number load array, even number load inverted array
    do 
    {
      if(i == 0)
        display.drawPixel(x0 - array[indexIncrement][1], y0 + array[indexIncrement][0], SSD1306_WHITE);
      if(i == 1)
        display.drawPixel(x0 - array[indexDecrement][1], y0 - array[indexDecrement][0], SSD1306_WHITE);
      if(i == 2)
        display.drawPixel(x0 - array[indexIncrement][0], y0 - array[indexIncrement][1], SSD1306_WHITE);
      if(i == 3)
        display.drawPixel(x0 + array[indexDecrement][0], y0 - array[indexDecrement][1], SSD1306_WHITE);
      if(i == 4)
      display.drawPixel(x0 + array[indexIncrement][1], y0 - array[indexIncrement][0], SSD1306_WHITE);
      if(i == 5)
        display.drawPixel(x0 + array[indexDecrement][1], y0 + array[indexDecrement][0], SSD1306_WHITE);
      
      //to fill holes in inner circle
      if(innerCircle)
      {
        if(i == 0)
        {
          display.drawPixel(x0 - array[indexIncrement][1], y0 + array[indexIncrement][0], SSD1306_WHITE);
          display.drawPixel(x0 - array[indexIncrement][1] - 1, y0 + array[indexIncrement][0], SSD1306_WHITE);
          display.drawPixel(x0 - array[indexIncrement][1] + 1, y0 + array[indexIncrement][0], SSD1306_WHITE);
        }
        
        if(i == 1)
        {
          display.drawPixel(x0 - array[indexDecrement][1], y0 - array[indexDecrement][0], SSD1306_WHITE);
          display.drawPixel(x0 - array[indexDecrement][1] + 1, y0 - array[indexDecrement][0], SSD1306_WHITE);
          display.drawPixel(x0 - array[indexDecrement][1] - 1, y0 - array[indexDecrement][0], SSD1306_WHITE);
        }

        if(i == 2)
        {
          display.drawPixel(x0 - array[indexIncrement][0], y0 - array[indexIncrement][1], SSD1306_WHITE);
          display.drawPixel(x0 - array[indexIncrement][0] + 1, y0 - array[indexIncrement][1], SSD1306_WHITE);
          display.drawPixel(x0 - array[indexIncrement][0] - 1, y0 - array[indexIncrement][1], SSD1306_WHITE);
        }
          
        if(i == 3)
        {
          display.drawPixel(x0 + array[indexDecrement][0], y0 - array[indexDecrement][1], SSD1306_WHITE);
          display.drawPixel(x0 + array[indexDecrement][0] + 1 , y0 - array[indexDecrement][1], SSD1306_WHITE);
          display.drawPixel(x0 + array[indexDecrement][0] - 1, y0 - array[indexDecrement][1], SSD1306_WHITE);

        }
          
        if(i == 4)
        {
          display.drawPixel(x0 + array[indexIncrement][1], y0 - array[indexIncrement][0], SSD1306_WHITE);
          display.drawPixel(x0 + array[indexIncrement][1] + 1, y0 - array[indexIncrement][0], SSD1306_WHITE);
          display.drawPixel(x0 + array[indexIncrement][1] - 1, y0 - array[indexIncrement][0], SSD1306_WHITE);
        }
        
        if(i == 5)
        {
          display.drawPixel(x0 + array[indexDecrement][1], y0 + array[indexDecrement][0], SSD1306_WHITE);
          display.drawPixel(x0 + array[indexDecrement][1] + 1, y0 + array[indexDecrement][0], SSD1306_WHITE);
          display.drawPixel(x0 + array[indexDecrement][1] - 1, y0 + array[indexDecrement][0], SSD1306_WHITE);
        }
          
      }
      
      //draw 2 point for full circle
      if(progress >= 16)
      {
        display.drawPixel(x0 - radius, y0, SSD1306_WHITE);
        display.drawPixel(x0 - radius, y0 + 1, SSD1306_WHITE);
      }
        
      if(progress >= 49)
      {
        display.drawPixel(x0, y0 - radius, SSD1306_WHITE);
        display.drawPixel(x0 - 1, y0 - radius, SSD1306_WHITE);
      }        
      if(progress >= 80)
      {
        display.drawPixel(x0 + radius, y0, SSD1306_WHITE);
        display.drawPixel(x0 + radius, y0 - 1, SSD1306_WHITE);
      }

      ++numOfActivePixels;
      ++indexIncrement;
      --indexDecrement;
      if(numOfActivePixels >= progressPixels)
        break;

    } while (indexDecrement > 0);
  }

}

void dispPemsVisualize(Adafruit_SSD1306 &display, uint8_t progress)
{
  int bigRadius = 37;
  int smallRadius = 28;
  String progressString = String(progress);

  int16_t cursorX = 0;
  int16_t cursorY = 40;

  //probably not needed vars
  uint8_t progressStringOffset = 15;
  int8_t physQtyOffset = -4;
  int8_t circleOffset = 5;

  if(progress < 10)
    progressStringOffset = 14;
  else if (progress >= 10 && progress < 100)
    progressStringOffset = 18;
  else
    progressStringOffset = 22;

  display.clearDisplay();
  
  /*display.drawString(DISP_CENTER_X0 + physQtyOffset, DISP_CENTER_Y0 - 5,  " MW");
  display.drawString(DISP_CENTER_X0 + physQtyOffset, DISP_CENTER_Y0 + 5,  " RPM");
  display.drawString(DISP_CENTER_X0 - progressStringOffset, DISP_CENTER_Y0 - 5, progressString);
  display.drawString(DISP_CENTER_X0 - progressStringOffset, DISP_CENTER_Y0 + 5, progressString);*/
 
  
  drawCirclePems(DISP_CENTER_X0, DISP_CENTER_Y0 + circleOffset, bigRadius, display, progress);
  drawCirclePems(DISP_CENTER_X0, DISP_CENTER_Y0 + circleOffset, bigRadius - 1, display, progress, true);
  drawCirclePems(DISP_CENTER_X0, DISP_CENTER_Y0 + circleOffset, bigRadius - 2, display, progress);
  drawCirclePems(DISP_CENTER_X0, DISP_CENTER_Y0 + circleOffset, smallRadius, display, progress);
  drawCirclePems(DISP_CENTER_X0, DISP_CENTER_Y0 + circleOffset, smallRadius + 1 , display, progress, true);
  drawCirclePems(DISP_CENTER_X0, DISP_CENTER_Y0 + circleOffset, smallRadius + 2, display, progress);



  display.setFont(&DejaVu_Sans_Mono_10);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 28);
  display.println(F("       1229   "));
  cursorY = display.getCursorY();
  display.setCursor(0, cursorY - 5);

  display.setFont(&DejaVu_Sans_Mono_8);
  display.setTextColor(SSD1306_WHITE);

  display.println  ("          kW    ");
  cursorY = display.getCursorY();
  display.setCursor(0, cursorY + 3);

  display.setFont(&DejaVu_Sans_Mono_10);
  display.setTextColor(SSD1306_WHITE);

  display.println(F("       1627"));
  cursorY = display.getCursorY();
  display.setCursor(0, cursorY - 5);

  display.setFont(&DejaVu_Sans_Mono_8);
  display.setTextColor(SSD1306_WHITE);

  display.println  ("         rpm    ");
  
  
  /*
  display.setFont(&DejaVu_Sans_Mono_10);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, cursorY);
  display.println(F("          MW"));
  display.println(F("          RPM"));

  int16_t cursorOffsetX = 45;
  display.setCursor(0, cursorY);
  cursorY = display.getCursorY();
  display.setCursor(cursorX + cursorOffsetX, cursorY);
  display.printf("%u", progress);
  display.println();

  cursorY = display.getCursorY();
  display.setCursor(cursorX + cursorOffsetX, cursorY);
  display.printf("%u", progress*2);*/

  display.display();
}

