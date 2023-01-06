#include <Wire.h>
#include <Arduino.h>
#include <PCF8574.h>
#include <stdbool.h>
#include <Adafruit_PWMServoDriver.h>
#include "../lib/Marine_panel.h"
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for SSD1306 display connected using software SPI (default case):
#define OLED_MOSI  10
#define OLED_CLK   11
#define OLED_DC    12
#define OLED_CS    13
#define OLED_RESET 14

#define OLED_CS2    2
#define OLED_RESET2 1

Adafruit_SSD1306 display1(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);
Adafruit_SSD1306 display2(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET2, OLED_CS2);

void testdrawline() {
  int16_t i;

  display1.clearDisplay(); // Clear display buffer

  for(i=0; i<display1.width(); i+=4) {
    display1.drawLine(0, 0, i, display1.height()-1, SSD1306_WHITE);
    display1.display(); // Update screen with each newly-drawn line
    delay(1);
  }
  for(i=0; i<display1.height(); i+=4) {
    display1.drawLine(0, 0, display1.width()-1, i, SSD1306_WHITE);
    display1.display();
    delay(1);
  }
  delay(250);

  display1.clearDisplay();

  for(i=0; i<display1.width(); i+=4) {
    display1.drawLine(0, display1.height()-1, i, 0, SSD1306_WHITE);
    display1.display();
    delay(1);
  }
  for(i=display1.height()-1; i>=0; i-=4) {
    display1.drawLine(0, display1.height()-1, display1.width()-1, i, SSD1306_WHITE);
    display1.display();
    delay(1);
  }
  delay(250);

  display1.clearDisplay();

  for(i=display1.width()-1; i>=0; i-=4) {
    display1.drawLine(display1.width()-1, display1.height()-1, i, 0, SSD1306_WHITE);
    display1.display();
    delay(1);
  }
  for(i=display1.height()-1; i>=0; i-=4) {
    display1.drawLine(display1.width()-1, display1.height()-1, 0, i, SSD1306_WHITE);
    display1.display();
    delay(1);
  }
  delay(250);

  display1.clearDisplay();

  for(i=0; i<display1.height(); i+=4) {
    display1.drawLine(display1.width()-1, 0, 0, i, SSD1306_WHITE);
    display1.display();
    delay(1);
  }
  for(i=0; i<display1.width(); i+=4) {
    display1.drawLine(display1.width()-1, 0, i, display1.height()-1, SSD1306_WHITE);
    display1.display();
    delay(1);
  }

  delay(2000); // Pause for 2 seconds
}

void testdrawline2() {
  int16_t i;

  display2.clearDisplay(); // Clear display buffer

  for(i=0; i<display1.width(); i+=4) {
    display2.drawLine(0, 0, i, display2.height()-1, SSD1306_WHITE);
    display2.display(); // Update screen with each newly-drawn line
    delay(1);
  }
  for(i=0; i<display2.height(); i+=4) {
    display2.drawLine(0, 0, display2.width()-1, i, SSD1306_WHITE);
    display2.display();
    delay(1);
  }
  delay(250);

  display2.clearDisplay();

  for(i=0; i<display2.width(); i+=4) {
    display2.drawLine(0, display2.height()-1, i, 0, SSD1306_WHITE);
    display2.display();
    delay(1);
  }
  for(i=display2.height()-1; i>=0; i-=4) {
    display2.drawLine(0, display2.height()-1, display2.width()-1, i, SSD1306_WHITE);
    display2.display();
    delay(1);
  }
  delay(250);

  display2.clearDisplay();

  for(i=display2.width()-1; i>=0; i-=4) {
    display2.drawLine(display2.width()-1, display2.height()-1, i, 0, SSD1306_WHITE);
    display2.display();
    delay(1);
  }
  for(i=display2.height()-1; i>=0; i-=4) {
    display2.drawLine(display2.width()-1, display2.height()-1, 0, i, SSD1306_WHITE);
    display2.display();
    delay(1);
  }
  delay(250);

  display2.clearDisplay();

  for(i=0; i<display2.height(); i+=4) {
    display2.drawLine(display2.width()-1, 0, 0, i, SSD1306_WHITE);
    display2.display();
    delay(1);
  }
  for(i=0; i<display2.width(); i+=4) {
    display2.drawLine(display2.width()-1, 0, i, display2.height()-1, SSD1306_WHITE);
    display2.display();
    delay(1);
  }

  delay(2000); // Pause for 2 seconds
}

void testdrawrect(void) {
  display1.clearDisplay();

  for(int16_t i=0; i<display1.height()/2; i+=2) {
    display1.drawRect(i, i, display1.width()-2*i, display1.height()-2*i, SSD1306_WHITE);
    display1.display(); // Update screen with each newly-drawn rectangle
    delay(1);
  }

  delay(2000);
}

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16
static const unsigned char PROGMEM logo_bmp[] =
{ 0b00000000, 0b11000000,
  0b00000001, 0b11000000,
  0b00000001, 0b11000000,
  0b00000011, 0b11100000,
  0b11110011, 0b11100000,
  0b11111110, 0b11111000,
  0b01111110, 0b11111111,
  0b00110011, 0b10011111,
  0b00011111, 0b11111100,
  0b00001101, 0b01110000,
  0b00011011, 0b10100000,
  0b00111111, 0b11100000,
  0b00111111, 0b11110000,
  0b01111100, 0b11110000,
  0b01110000, 0b01110000,
  0b00000000, 0b00110000 };

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

   // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display1.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

   if(!display2.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  //displays
  display1.display();
  display2.display();
  delay(2000); // Pause for 2 seconds

    // Clear the buffer
  display1.clearDisplay();
  display2.clearDisplay();

  // Draw a single pixel in white
  display1.drawPixel(10, 10, SSD1306_WHITE);

  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display1.display();
  display2.display();
  delay(2000);
  // display1.display() is NOT necessary after every single drawing command,
  // unless that's what you want...rather, you can batch up a bunch of
  // drawing operations and then update the screen all at once by calling
  // display1.display(). These examples demonstrate both approaches...

  testdrawline();      // Draw many lines
  testdrawline2();


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

