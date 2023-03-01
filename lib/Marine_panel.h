#ifndef MARINE_PANEL_H
#define MARINE_PANEL_H

//PCF modules from different manufacturers may have different start up adresses!!! Needs to be verified by I2C scanner
#define PCF1_ADRESS 0x38    
#define PCF2_ADRESS 0x21
#define PCF3_ADRESS 0x22
#define PCF4_ADRESS 0x23
#define PCF5_ADRESS 0x24
#define PCF6_ADRESS 0x25
#define PCF7_ADRESS 0x26
#define PCF8_ADRESS 0x27

#define PWM1_ADRESS 0x40
#define PWM2_ADRESS 0x41
#define PWM3_ADRESS 0x42

#define DISP_CENTER_X0 64
#define DISP_CENTER_Y0 32

//Modbus
#define RST 0
#define PORT 502

// Declaration for SSD1306 display connected using software SPI (default case):
#define DISP_DC     33

#define DISP1_CS    1
#define DISP2_CS    2
#define DISP3_CS    3
#define DISP4_CS    4
#define DISP5_CS    5
#define DISP6_CS    6
#define DISP7_CS    7
#define DISP8_CS    10
#define DISP9_CS    11
#define DISP10_CS   12
#define DISP11_CS   13
#define DISP12_CS   14
#define DISP13_CS   15
#define DISP14_CS   16
#define DISP15_CS   45


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels




#include <Wire.h>
#include <Arduino.h>
#include <PCF8574.h>
#include <stdbool.h>
#include <Adafruit_PWMServoDriver.h>
#include <SSD1306Spi.h>
#include <OLEDDisplay.h>
#include "Images.h"

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


/* 
IN: 
  pin1 is 1st pin pcf, read 1st state of button
  pin2 is 2nd pin pcf, read 2nd state of button
OUT
  state = 0 -> no input
  state = 1 -> 1st state
  state = 2 -> 2nd state
*/
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


void drawCirclePems(int16_t x0, int16_t y0, int16_t radius, SSD1306Spi &display, uint8_t progress, bool innerCircle = false) {
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
        display.setPixel(x0 - array[indexIncrement][1], y0 + array[indexIncrement][0]);
      if(i == 1)
        display.setPixel(x0 - array[indexDecrement][1], y0 - array[indexDecrement][0]);
      if(i == 2)
        display.setPixel(x0 - array[indexIncrement][0], y0 - array[indexIncrement][1]);
      if(i == 3)
        display.setPixel(x0 + array[indexDecrement][0], y0 - array[indexDecrement][1]);
      if(i == 4)
      display.setPixel(x0 + array[indexIncrement][1], y0 - array[indexIncrement][0]);
      if(i == 5)
        display.setPixel(x0 + array[indexDecrement][1], y0 + array[indexDecrement][0]);
      
      //to fill holes in inner circle
      if(innerCircle)
      {
        if(i == 0)
        {
          display.setPixel(x0 - array[indexIncrement][1], y0 + array[indexIncrement][0]);
          display.setPixel(x0 - array[indexIncrement][1] - 1, y0 + array[indexIncrement][0]);
          display.setPixel(x0 - array[indexIncrement][1] + 1, y0 + array[indexIncrement][0]);
        }
        
        if(i == 1)
        {
          display.setPixel(x0 - array[indexDecrement][1], y0 - array[indexDecrement][0]);
          display.setPixel(x0 - array[indexDecrement][1] + 1, y0 - array[indexDecrement][0]);
          display.setPixel(x0 - array[indexDecrement][1] - 1, y0 - array[indexDecrement][0]);
        }

        if(i == 2)
        {
          display.setPixel(x0 - array[indexIncrement][0], y0 - array[indexIncrement][1]);
          display.setPixel(x0 - array[indexIncrement][0] + 1, y0 - array[indexIncrement][1]);
          display.setPixel(x0 - array[indexIncrement][0] - 1, y0 - array[indexIncrement][1]);
        }
          
        if(i == 3)
        {
          display.setPixel(x0 + array[indexDecrement][0], y0 - array[indexDecrement][1]);
          display.setPixel(x0 + array[indexDecrement][0] + 1 , y0 - array[indexDecrement][1]);
          display.setPixel(x0 + array[indexDecrement][0] - 1, y0 - array[indexDecrement][1]);

        }
          
        if(i == 4)
        {
          display.setPixel(x0 + array[indexIncrement][1], y0 - array[indexIncrement][0]);
          display.setPixel(x0 + array[indexIncrement][1] + 1, y0 - array[indexIncrement][0]);
          display.setPixel(x0 + array[indexIncrement][1] - 1, y0 - array[indexIncrement][0]);
        }
        
        if(i == 5)
        {
          display.setPixel(x0 + array[indexDecrement][1], y0 + array[indexDecrement][0]);
          display.setPixel(x0 + array[indexDecrement][1] + 1, y0 + array[indexDecrement][0]);
          display.setPixel(x0 + array[indexDecrement][1] - 1, y0 + array[indexDecrement][0]);
        }
          
      }
      
      //draw 2 point for full circle
      if(progress >= 16)
      {
        display.setPixel(x0 - radius, y0);
        display.setPixel(x0 - radius, y0 + 1);
      }
        
      if(progress >= 49)
      {
        display.setPixel(x0, y0 - radius);
        display.setPixel(x0 - 1, y0 - radius);
      }        
      if(progress >= 80)
      {
        display.setPixel(x0 + radius, y0);
        display.setPixel(x0 + radius, y0 - 1);
      }

      ++numOfActivePixels;
      ++indexIncrement;
      --indexDecrement;
      if(numOfActivePixels >= progressPixels)
        break;

    } while (indexDecrement > 0);
  }

}

void dispPemsVisualize(SSD1306Spi &display, uint8_t progress)
{
  int bigRadius = 37;
  int smallRadius = 28;
  String progressString = String(progress);
  uint8_t progressStringOffset = 15;
  int8_t physQtyOffset = -4;
  int8_t circleOffset = 5;

  if(progress < 10)
    progressStringOffset = 14;
  else if (progress >= 10 && progress < 100)
    progressStringOffset = 18;
  else
    progressStringOffset = 22;

  display.drawString(DISP_CENTER_X0 + physQtyOffset, DISP_CENTER_Y0 - 5,  " MW");
  display.drawString(DISP_CENTER_X0 + physQtyOffset, DISP_CENTER_Y0 + 5,  " RPM");
  display.drawString(DISP_CENTER_X0 - progressStringOffset, DISP_CENTER_Y0 - 5, progressString);
  display.drawString(DISP_CENTER_X0 - progressStringOffset, DISP_CENTER_Y0 + 5, progressString);
 
  
  drawCirclePems(DISP_CENTER_X0, DISP_CENTER_Y0 + circleOffset, bigRadius, display, progress);
  drawCirclePems(DISP_CENTER_X0, DISP_CENTER_Y0 + circleOffset, bigRadius - 1, display, progress, true);
  drawCirclePems(DISP_CENTER_X0, DISP_CENTER_Y0 + circleOffset, bigRadius - 2, display, progress);
  drawCirclePems(DISP_CENTER_X0, DISP_CENTER_Y0 + circleOffset, smallRadius, display, progress);
  drawCirclePems(DISP_CENTER_X0, DISP_CENTER_Y0 + circleOffset, smallRadius + 1 , display, progress, true);
  drawCirclePems(DISP_CENTER_X0, DISP_CENTER_Y0 + circleOffset, smallRadius + 2, display, progress);
}

void dispInit(SSD1306Spi &display)
{
  display.init();
  display.flipScreenVertically();
  display.setContrast(255);
  delay(10);
}

void dispRCSAzipodVisualize(SSD1306Spi &display, SSD1306Spi &display2, SSD1306Spi &display3)
{
  //RPM
  uint8_t dispXOffset = 3;
  uint8_t dispYOffset = 10;
  display.clear();
  display.setFont(ArialMT_Plain_10);
  display.drawString(dispXOffset, dispYOffset-6, "RPM");
  display.setFont(ArialMT_Plain_10);
  display.drawString(dispXOffset, dispYOffset + 12, "100 port");
  display.drawString(dispXOffset + 58, dispYOffset + 12, "0");
  display.drawString(dispXOffset + 80, dispYOffset + 12, "stbd 100");
  display.drawString(dispXOffset + 55, dispYOffset - 5, "port");
  display.drawString(dispXOffset + 110, dispYOffset - 5, "%");
  display.setFont(ArialMT_Plain_16);
  display.drawString(dispXOffset + 78, dispYOffset - 10, "60.0");
  //display.drawString(dispXOffset + 83, dispYOffset - 5, "%");
  display.drawProgressBar(dispXOffset, dispYOffset + 27, 120, 20, 70);
  display.display();

  //Angle
  display2.clear();
  display2.drawXbm(0, 0, thrustWithcircle_width, thrustWithcircle_height, thrustWithcircle);
  display2.setFont(ArialMT_Plain_16);
  display2.drawString(0, 0, "150°");
  display2.setFont(ArialMT_Plain_10);
  display2.drawString(0, 15, "DEG actual");
  display2.setFont(ArialMT_Plain_16);
  display2.drawString(0, 37, "100°");
  display2.setFont(ArialMT_Plain_10);
  display2.drawString(0, 52, "DEG reference");
  display2.display();

  //Power
  uint8_t disp3XOffset = 3;
  uint8_t disp3YOffset = 10;
  display3.clear();
  display3.setFont(ArialMT_Plain_10);
  display3.drawString(disp3XOffset, disp3YOffset-6, "POWER");
  display3.setFont(ArialMT_Plain_10);
  display3.drawString(disp3XOffset + 2, disp3YOffset + 12, "0");
  display3.drawString(disp3XOffset + 105, disp3YOffset + 12, "1.1");
  display3.drawString(disp3XOffset + 103, disp3YOffset - 5, "MW");
  display3.setFont(ArialMT_Plain_16);
  display3.drawString(disp3XOffset + 78, disp3YOffset - 10, "0.2");
  display3.drawProgressBar(disp3XOffset, disp3YOffset + 27, 120, 20, 70);
  display3.display();
}

void dispAlarmVisualize(SSD1306Spi &display, SSD1306Spi &display2, SSD1306Spi &display3, SSD1306Spi &display4)
{
 
  //time and date
  uint8_t dispXOffset = 0;
  uint8_t dispYOffset = 20;
  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.drawString(dispXOffset, 0 * dispYOffset, "20-10-2022 08:49");
  display.drawString(dispXOffset, 1 * dispYOffset, "20-10-2022 08:48");
  display.drawString(dispXOffset, 2 * dispYOffset, "20-10-2022 08:47");
  display.display();

 
  display2.clear();
  display2.setFont(ArialMT_Plain_16);
  display2.drawString(dispXOffset, 0 * dispYOffset, "VMS Pump1");
  display2.drawString(dispXOffset, 1 * dispYOffset, "VMS Pump2");
  display2.drawString(dispXOffset, 2 * dispYOffset, "VMS Pump3");
  display2.display();

  display3.clear();
  display3.setFont(ArialMT_Plain_16);
  display3.drawString(dispXOffset, 0 * dispYOffset, "Power failure");
  display3.drawString(dispXOffset, 1 * dispYOffset, "Power failure");
  display3.drawString(dispXOffset, 2 * dispYOffset, "Power failure");
  display3.display();

  display4.clear();
  display4.setFont(ArialMT_Plain_16);
  display4.drawString(dispXOffset, 0 * dispYOffset, "Text Text Text");
  display4.drawString(dispXOffset, 1 * dispYOffset, "Text Text Text");
  display4.drawString(dispXOffset, 2 * dispYOffset, "Text Text Text");
  display4.display();
}

void mbTCPInit()
{
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




#endif