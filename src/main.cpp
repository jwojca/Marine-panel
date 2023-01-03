#include <Wire.h>
#include <Arduino.h>
#include <PCF8574.h>
#include <stdbool.h>
#include <Adafruit_PWMServoDriver.h>
#include "../lib/Marine_panel.h"


PCF8574 pcf(PCF_ADRESS);
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(PWM_ADRESS);


void setup()
{
	Serial.begin(9600);
	delay(1000);

	// Set pinMode to OUTPUT
	pcf.pinMode(P0, INPUT);
  pcf.pinMode(P1, INPUT);

	Serial.print("Init pcf8574...");
	if (pcf.begin())
		Serial.println("OK");
  else
		Serial.println("KO");

  //PWM
  pwm.begin();
  pwm.setPWMFreq(50);
}

void loop()
{
	uint8_t state = read3State(P0, P1, true, pcf);
  if(state == 0)
    RGBLedColor(0, 255, 0, 0, pwm);
  else if(state == 1)
    RGBLedColor(0, 0, 255, 0, pwm);
  else if(state == 2)
    RGBLedColor(0, 0, 0, 255, pwm);

  delay(10);
}

