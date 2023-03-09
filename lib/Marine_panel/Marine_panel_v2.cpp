#include "Marine_panel_v2.h"

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



void Valve::fail()
{
  //calculate first pin of pwm channel based on RGB number
  uint8_t firstPin = (rgbNumber % 6) * 3;
  RGBLedColor(firstPin, 255, 0, 0, pwm);
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
  uint8_t state = read3State(pcf1Pin, false, *pcf1);
  if(state == 1)
    this->valveState = Failure;
  else
  {
    if(this->valveMode == Local)
    {
        bool state2 = read2State(pcf2Pin, false, *pcf2);
        if(state2)
            this->valveState = Opened;
        else
            this->valveState = Closed;
    }
    else    //Auto - read from modbus
    {

    }
    
  }

}

void Valve::writeCmd()
{
  //calculate first pin of pwm channel based on RGB number
  uint8_t firstPin = (rgbNumber % 6) * 3;
  if(this->valveState == Failure)
    RGBLedColor(firstPin, 255, 0, 0, pwm);
  else
  {
    if(this->valveState == Opened)
        RGBLedColor(firstPin, 0, 255, 0, pwm);
    if(this->valveState == Closed)
        RGBLedColor(firstPin, 0, 0, 0, pwm);
  }
  
  
}

void Pump::start()
{
  //calculate first pin of pwm channel based on RGB number
  uint8_t firstPin = (rgbNumber % 6) * 3;
  RGBLedColor(firstPin, 0, 255, 0, pwm);
}

void Pump::stop()
{
  //calculate first pin of pwm channel based on RGB number
  uint8_t firstPin = (rgbNumber % 6) * 3;
  RGBLedColor(firstPin, 255, 0, 0, pwm);
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
  //Pressure oscilation
  float pNoise1 = (float)random(-5, 5)/100;
  float pNoise2 = (float)random(-3, 3)/100;
  Pump1.pressure += pNoise1;
  Pump2.pressure += pNoise2;
  float dp = (Pump2.pressure - vmsSimVars.PressureAct) * 0.1;
  float dt = (float)task/1000;
  vmsSimVars.Inflow = dt * Valve1.valveState * Pump2.maxInflow * dp; 

  vmsSimVars.TankWater = constrain(vmsSimVars.TankWater + vmsSimVars.Inflow - vmsSimVars.Outflow, 0, vmsSimVars.TankMaxVol);
  //Serial.println(gVmsTankWater);

  //pressure equation
  //gVmsPressureAct = exp((gVmsTankWater/1000) - 7);
  //gVmsPressureAct = (gVmsMaxPressure * 1000)/(gVmsTankMaxVol - gVmsTankWater);
  vmsSimVars.PressureAct = (vmsSimVars.MaxPressure * 1000) / (vmsSimVars.TankMaxVol - vmsSimVars.TankWater);
}