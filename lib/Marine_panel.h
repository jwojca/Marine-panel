#ifndef MARINE_PANEL_H
#define MARINE_PANEL_H

#define PCF_ADRESS 0x20
#define PWM_ADRESS 0x40

#include <Wire.h>
#include <Arduino.h>
#include <PCF8574.h>
#include <stdbool.h>
#include <Adafruit_PWMServoDriver.h>
#include <SSD1306Spi.h>
#include <OLEDDisplay.h>

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

void dispPemsVisualize(SSD1306Spi &display)
{
  int bigRadius = 31;
  int smallRadius = 25;
  display.drawString((display.getWidth()/2)-15, (display.getHeight()/2)-10, "0 MW");
  display.drawString((display.getWidth()/2)-15, (display.getHeight()/2), "0 RPM");
  display.display();
  for(uint16_t i = 1; i < 3; ++i)
  {
    display.drawCircleQuads(display.getWidth()/2, display.getHeight()/2, smallRadius, i);
    display.drawCircleQuads(display.getWidth()/2, display.getHeight()/2, bigRadius, i);
    delay(500);
    display.display();
  }
  display.drawCircle(display.getWidth()/2, display.getHeight()/2, smallRadius);
  display.drawCircle(display.getWidth()/2, display.getHeight()/2, bigRadius);
  delay(500);
  display.display();
  delay(500);
  display.clear();
  delay(100);
}

void dispPemsVisualize2(SSD1306Spi &display)
{
  int bigRadius = 31;
  int smallRadius = 25;
  display.drawString((display.getWidth()/2)-15, (display.getHeight()/2)-10, "0 MW");
  display.drawString((display.getWidth()/2)-15, (display.getHeight()/2), "0 RPM");
  display.display();

  display.drawCircleQuads(display.getWidth()/2, display.getHeight()/2, smallRadius, 0x01);
  display.drawCircleQuads(display.getWidth()/2, display.getHeight()/2, bigRadius, 0x01);
  delay(500);
  display.display();

  display.drawCircleQuads(display.getWidth()/2, display.getHeight()/2, smallRadius, 0x02);
  display.drawCircleQuads(display.getWidth()/2, display.getHeight()/2, bigRadius, 0x02);
  delay(500);
  display.display();

  display.drawCircleQuads(display.getWidth()/2, display.getHeight()/2, smallRadius, 0x04);
  display.drawCircleQuads(display.getWidth()/2, display.getHeight()/2, bigRadius, 0x04);
  delay(500);
  display.display();

  display.drawCircleQuads(display.getWidth()/2, display.getHeight()/2, smallRadius, 0x08);
  display.drawCircleQuads(display.getWidth()/2, display.getHeight()/2, bigRadius, 0x08);
  delay(500);
  display.display();
  display.clear();
}

void drawCirclePems(int16_t x0, int16_t y0, int16_t radius, SSD1306Spi &display, uint8_t progress) {
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
      
      //draw 2 point for full circle
      if(progress > 16)
      {
        display.setPixel(x0 - radius, y0);
        display.setPixel(x0 - radius, y0 + 1);
      }
        
      if(progress > 49)
      {
        display.setPixel(x0, y0 - radius);
        display.setPixel(x0 - 1, y0 - radius);
      }        
      if(progress > 82)
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


#endif