#include <Wire.h>
#include <Arduino.h>
#include <PCF8574.h>
#include <stdbool.h>
#include <Adafruit_PWMServoDriver.h>
#include "../lib/Marine_panel.h"
#include <SPI.h>
#include "SSD1306Spi.h"



// Declaration for SSD1306 display connected using software SPI (default case):
#define OLED_DC    12
#define OLED_CS    13
#define OLED_RESET 14
#define OLED_CS2    6
#define OLED_RESET2 5

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

SSD1306Spi display(OLED_RESET, OLED_DC, OLED_CS);
SSD1306Spi display2(OLED_RESET2, OLED_DC, OLED_CS2);
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

  String text1 = "Frequency: 50 Hz";
  String text2 = "Power: 600 kW";
  String text3 = "Voltage: 1000 V";
  // put your setup code here, to run once:
  display.init();
  display.flipScreenVertically();
  display.setContrast(255);
 
  display2.init();
  display2.flipScreenVertically();
  display2.setContrast(255);

  display.setFont(ArialMT_Plain_16);
  display.drawStringMaxWidth(0,0, display.getWidth(),text1);
  display.drawStringMaxWidth(0,20, display.getWidth(),text2);
  display.drawStringMaxWidth(0,40, display.getWidth(),text3);
  display.display();
  delay(1000);
  display.clear();
  display.setFont(ArialMT_Plain_10); 
}

void loop()
{
	/*uint8_t state = read3State(P0, P1, true, pcf);
  if(state == 0)
    RGBLedColor(0, 255, 0, 0, pwm);
  else if(state == 1)
    RGBLedColor(0, 0, 255, 0, pwm);
  else if(state == 2)
    RGBLedColor(0, 0, 0, 255, pwm);
  delay(10);*/

  display.clear();
  for(uint8_t i = 0; i < 100; ++i)
  {
    display.drawProgressBar(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 60, 30, i);
    display.display();
    delay(20);
  }
  //dispPemsVisualize2(display2);
  int16_t x0 = SCREEN_WIDTH/2;
  int16_t y0 = SCREEN_HEIGHT/2;
  int16_t r = 20;
/*
  display2.setPixel(x0 - r, y0);
  delay(1000);
  display2.display();
  display2.setPixel(x0 - 1, y0 - r);
  display2.setPixel(x0, y0 - r);
  delay(1000);
  display2.display();
  display2.setPixel(x0 + r, y0);
  delay(1000);
  display2.display();*/

  for(uint16_t i = 0; i < 50; ++i)
  {
    drawCirclePems(x0, y0, r, display2, i*2);
    delay(20);
    display2.display();
    
  }
  

  
  display2.clear();
  

}

