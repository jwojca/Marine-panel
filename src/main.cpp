
#include <Wire.h>
#include <Arduino.h>
#include <PCF8574.h>
#include <stdbool.h>
#include <Adafruit_PWMServoDriver.h>
#include "../lib/Marine_panel.h"


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
    RGBLedColor(0, 255, 0, 0, pwm);
  else if(state == 1)
    RGBLedColor(0, 0, 255, 0, pwm);
  else if(state == 2)
    RGBLedColor(0, 0, 0, 255, pwm);

  delay(1000);
  RGBLedOff(0, pwm);
  delay(1000);
}

