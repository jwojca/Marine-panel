<<<<<<< HEAD
#include <SPI.h>
#include <Ethernet.h>
// nastavení MAC adresy
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
// nastavení IP adresy - musí být přidělitelná DHCP serverem,
// tedy ve správném rozsahu, případně přidělena routerem ručně
IPAddress ip(192, 168, 43, 50);
// inicializace serveru na portu 80
EthernetServer server(80);

void setup() {
  // inicializace komunikace po sériové lince rychlostí 9600 baud
  Serial.begin(9600);
  // zapnutí komunikace s Ethernet Shieldem
  Ethernet.begin(mac, ip);
  server.begin();
  // výpis informace o nastavené IP adrese
  Serial.print("Server je na IP adrese: ");
  Serial.println(Ethernet.localIP());
}

void loop() {
  // načtení připojených klientů
  EthernetClient klient = server.available();
  // pokud se připojí nějaký klient, provedeme následující
  if (klient) {
    Serial.println("Novy klient:");
    // http request končí prázdným řádkem
    boolean aktualniRadkaPrazdna = true;
    // pokud je klient připojen a k dispozici,
    // vytiskneme mu dostupná data
    while (klient.connected()) {
      if (klient.available()) {
        // načtení a vytištění informace o klientovi
        char c = klient.read();
        Serial.write(c);
        // pokud se dostaneme na konec řádku a následující je prázdný,
        // byl ukončen request a my můžeme poslat odezvu
        if (c == '\n' && aktualniRadkaPrazdna) {
          // nejprve pošleme standardní http odezvu
          klient.println("HTTP/1.1 200 OK");
          klient.println("Content-Type: text/html");
          klient.println("Connection: close");
          // nastavení automatické obnovy stránky po 5 vteřinách
          klient.println("Refresh: 5");
          klient.println();
          klient.println("");
          klient.println("");
          // místo pro tištění vlastních údajů
          klient.print("Cas od spusteni: ");
          klient.print(millis() / 1000);
          klient.print(" vterin.<br />");
          // ukázka výpisu dat ze všech analogových vstupů
          for (int analogPin = 0; analogPin < 6; analogPin++) {
            int analogData = analogRead(analogPin);
            klient.print("Analogovy vstup A");
            klient.print(analogPin);
            klient.print(": ");
            klient.print(analogData);
            // příkaz "<br />" funguje jako krok na další řádek
            klient.println("<br />");
          }
          klient.println("");
          break;
        }
        // začátek nové řádky
        if (c == '\n') {
          aktualniRadkaPrazdna = true;
          // detekce znaku na nové řádce
        } else if (c != '\r') {
          aktualniRadkaPrazdna = false;
        }
      }
    }
    // pauza pro prohlížeč, aby stihl zpracovat všechny data
    delay(1);
    // uzavření spojení
    klient.stop();
    Serial.println("Klient odpojen.");
    Serial.println("---------------------");
  }
  else
    Serial.println("Not connected");
}
=======
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
>>>>>>> parent of 24d3ccd (First modbus test)
