#ifndef MARINE_PANEL_H
#define MARINE_PANEL_H

#define PCF_ADRESS 0x20
#define PWM_ADRESS 0x40

#include <Wire.h>
#include <Arduino.h>
#include <PCF8574.h>
#include <stdbool.h>
#include <Adafruit_PWMServoDriver.h>

void RGBLedOff(uint8_t firstPin, Adafruit_PWMServoDriver pwm)
{
  pwm.setPin(firstPin, 0);
  pwm.setPin(firstPin + 1, 0);
  pwm.setPin(firstPin + 2, 0);
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

void RGBLedBlink(uint16_t red, uint16_t green, uint16_t blue, uint16_t onDuration, uint16_t offDuration)
{

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

uint8_t read3State(uint8_t pin1, uint8_t pin2, bool printOn, PCF8574 pcf8574)
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


#endif