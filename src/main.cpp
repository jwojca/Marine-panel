
#define PCF_ADRESS 0x20
#define PWM_ADRESS 0x40

#include <Wire.h>
#include <Arduino.h>
#include <PCF8574.h>
#include <stdbool.h>
#include <Adafruit_PWMServoDriver.h>

PCF8574 pcf8574(PCF_ADRESS);
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(PWM_ADRESS);


bool read2State(uint8_t pin, bool printOn)
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

uint8_t read3State(uint8_t pin1, uint8_t pin2, bool printOn)
{
  uint8_t val1 = pcf8574.digitalRead(pin1);
  uint8_t val2 = pcf8574.digitalRead(pin2);
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

void RGBLedBlink(uint16_t red, uint16_t green, uint16_t blue, uint16_t onDuration, uint16_t offDuration)
{

}

void RGBLedOff(uint8_t firstPin)
{
  pwm.setPin(firstPin, 0);
  pwm.setPin(firstPin + 1, 0);
  pwm.setPin(firstPin + 2, 0);
}

void RGBLedColor(uint8_t afirstPin, uint8_t aRed, uint8_t aGreen, uint8_t aBlue)
{
  uint16_t red, green, blue;
  red = map(aRed, 0, 255, 0, 4095);
  green = map(aGreen, 0, 255, 0, 4095);
  blue = map(aBlue, 0, 255, 0, 4095);

  pwm.setPin(afirstPin, red);
  pwm.setPin(afirstPin + 1, green);
  pwm.setPin(afirstPin + 2, blue);
}



void setup()
{
	Serial.begin(9600);
	delay(1000);

	// Set pinMode to OUTPUT
	pcf8574.pinMode(P0, INPUT);
  pcf8574.pinMode(P1, INPUT);

	Serial.print("Init pcf8574...");
	if (pcf8574.begin())
		Serial.println("OK");
  else
		Serial.println("KO");

  //PWM
  pwm.begin();
  pwm.setPWMFreq(50);
}

void loop()
{
	uint8_t state = read3State(P0, P1, true);
  if(state == 0)
    RGBLedColor(0, 255, 0, 0);
  else if(state == 1)
    RGBLedColor(0, 0, 255, 0);
  else if(state == 2)
    RGBLedColor(0, 0, 0, 255);

  delay(100);
}

