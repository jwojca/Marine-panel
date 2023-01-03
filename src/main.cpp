#include <Wire.h>
#include "SSD1306Wire.h"

// for 128x64 displays:
SSD1306Wire display(0x3c, 8, 9);  // ADDRESS, SDA, SCL
// By default SH1106Wire set I2C frequency to 700000, you can use set either another frequency or skip setting the frequency by providing -1 value
// SH1106Wire(0x3c, SDA, SCL, GEOMETRY_128_64, I2C_ONE, 400000); //set I2C frequency to 400kHz
// SH1106Wire(0x3c, SDA, SCL, GEOMETRY_128_64, I2C_ONE, -1); //skip setting the I2C bus frequency


void setup() {
  String text1 = "Frequency: 50 Hz";
  String text2 = "Power: 600 kW";
  String text3 = "Voltage: 1000 V";
  // put your setup code here, to run once:
  display.init();
  display.setContrast(255);
  display.setFont(ArialMT_Plain_16);
  display.drawStringMaxWidth(0,0, display.getWidth(),text1);
  display.drawStringMaxWidth(0,20, display.getWidth(),text2);
  display.drawStringMaxWidth(0,40, display.getWidth(),text3);
  display.display();
  delay(10000);
  display.clear();
  display.setFont(ArialMT_Plain_10); 
}

void loop() {
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

/*#include <Wire.h>
#include<Arduino.h>>


void setup()
{
  Wire.begin();

  Serial.begin(9600);
  while (!Serial);             // Leonardo: wait for serial monitor
  Serial.println("\nI2C Scanner");
}


void loop()
{
  byte error, address;
  int nDevices;

  Serial.println("Scanning...");

  nDevices = 0;
  for(address = 1; address < 127; address++ ) 
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address<16) 
        Serial.print("0");
      Serial.print(address,HEX);
      Serial.println("  !");

      nDevices++;
    }
    else if (error==4) 
    {
      Serial.print("Unknown error at address 0x");
      if (address<16) 
        Serial.print("0");
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");

  delay(5000);           // wait 5 seconds for next scan
}
*/