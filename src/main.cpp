#include <Wire.h>
#include <Arduino.h>
#include <PCF8574.h>
#include <stdbool.h>
#include <Adafruit_PWMServoDriver.h>
#include "../lib/Marine_panel/Marine_panel_v2.h"
#include "../lib/HVAC/HVAC.h"
#include "../lib/PEMS/PEMS.h"


#include <SPI.h> 
#include <Ethernet.h>       // Ethernet library v2 is required
#include <ModbusAPI.h>
#include <ModbusTCPTemplate.h>


//rtc
RTC_DS1307 rtc;
bool setTime = false;






//peripherals
PCF8574 pcf1(PCF1_ADRESS); 
PCF8574 pcf2(PCF2_ADRESS); 
PCF8574 pcf3(PCF3_ADRESS); 
PCF8574 pcf4(PCF4_ADRESS); 
PCF8574 pcf5(PCF5_ADRESS);
PCF8574 pcf6(PCF6_ADRESS);


// Declaration for SSD1306 display connected using software SPI (default case):
/*#define OLED_MOSI   19
#define OLED_CLK   20
#define OLED_DC    18
#define OLED_CS    39 //++
#define OLED_CS2    0 //++
#define OLED_RESET 17 //--*/

Adafruit_SSD1306 display1(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, DISP_DC, DISP_RESET, DISP1_CS);
Adafruit_SSD1306 display2(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, DISP_DC, DISP_RESET, DISP2_CS);
Adafruit_SSD1306 display3(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, DISP_DC, DISP_RESET, DISP3_CS);
Adafruit_SSD1306 display4(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, DISP_DC, DISP_RESET, DISP4_CS);
Adafruit_SSD1306 display5(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, DISP_DC, DISP_RESET, DISP5_CS);
Adafruit_SSD1306 display6(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, DISP_DC, DISP_RESET, DISP6_CS);
Adafruit_SSD1306 display7(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, DISP_DC, DISP_RESET, DISP7_CS);
Adafruit_SSD1306 display8(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, DISP_DC, DISP_RESET, DISP8_CS);
Adafruit_SSD1306 display9(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, DISP_DC, DISP_RESET, DISP9_CS);
Adafruit_SSD1306 display10(SCREEN_WIDTH, SCREEN_HEIGHT, SPI_MOSI2, SPI_CLK2, DISP_DC2,  DISP_RESET, DISP10_CS);
Adafruit_SSD1306 display11(SCREEN_WIDTH, SCREEN_HEIGHT, SPI_MOSI2, SPI_CLK2, DISP_DC2,  DISP_RESET, DISP11_CS);
Adafruit_SSD1306 display12(SCREEN_WIDTH, SCREEN_HEIGHT, SPI_MOSI2, SPI_CLK2, DISP_DC2,  DISP_RESET, DISP12_CS);
Adafruit_SSD1306 display13(SCREEN_WIDTH, SCREEN_HEIGHT, SPI_MOSI2, SPI_CLK2, DISP_DC2,  DISP_RESET, DISP13_CS);
Adafruit_SSD1306 display14(SCREEN_WIDTH, SCREEN_HEIGHT, SPI_MOSI2, SPI_CLK2, DISP_DC2, DISP_RESET, DISP14_CS);
Adafruit_SSD1306 display15(SCREEN_WIDTH, SCREEN_HEIGHT, SPI_MOSI2, SPI_CLK2, DISP_DC2, DISP_RESET, DISP15_CS);

alarmDispsStruct alarmDisps{&display1, &display2, &display3, &display4};

Adafruit_PWMServoDriver pwm1 = Adafruit_PWMServoDriver(PWM1_ADRESS);
Adafruit_PWMServoDriver pwm2 = Adafruit_PWMServoDriver(PWM2_ADRESS);
Adafruit_PWMServoDriver pwm3 = Adafruit_PWMServoDriver(PWM3_ADRESS);

//Global variables 
/*float gVmsPump1Pressure = 7.8, gVmsPump2Pressure = 12.0, gVmsPressureRef = 7.0, gVmsMaxPressure = 15.0;
float gVmsTankWater = 5000.0, gVmsInflow = 0.0, gVmsOutflow = 0.0;
float gVmsPump2MaxInflow = 1600.0; //l/s
float gVmsTankMaxVol = 10000.0; //l
float gVmsPressureAct = 0.0; //Bar*/

vmsSimVarsStruct gVmsSimVars;
rcsVarsStruct grcsVars;
hvacSimVarsStruct gHvacSimVars
{
  .pressureRef = 1600.0, .pressure = 1500.0, .pressMin = 0.0, .pressMax = 2000.0,      //Pa
  .tempRef = 21.0, .temp = 21.0, .tempMin = 18.0, .tempMax = 30.0,                     //°C
  .roomVolume = 10000.0, .airInRoom = 0.0                                              //m3
};

uint16_t gVmsPump1Speed = 566;
Valve gValve1(&rtc, &alarmDisps, vmsV1Alarm1, pwm2, RGB8, P0, P0, &pcf4, &pcf5), gValve2(&rtc, &alarmDisps, vmsV2Alarm1, pwm2, RGB10, P2, P1, &pcf4, &pcf5);
Pump gPump1(&rtc, &alarmDisps, vmsP1Alarm1, pwm2, RGB9, P4, P2, &pcf4, &pcf5), gPump2(&rtc, &alarmDisps, vmsP2Alarm1, pwm2, RGB7, P6, P3, &pcf4, &pcf5);

Breaker gBreaker1(&rtc, &alarmDisps, pemsCB1Alarm1, pwm1, RGB2, P4, P2, &pcf1, &pcf3), gBreaker2(&rtc, &alarmDisps, pemsCB2Alarm1, pwm1, RGB5, P6, P3, &pcf1, &pcf3);
Breaker gBreaker3(&rtc, &alarmDisps, pemsCB3Alarm1, pwm1, RGB3, P0, P4, &pcf2, &pcf3), gBreaker4(&rtc, &alarmDisps, pemsCB4Alarm1, pwm1, RGB4, P2, P5, &pcf2, &pcf3);
Breaker gBreaker5(&rtc, &alarmDisps, pemsCB5Alarm1, pwm1, RGB1, P4, P6, &pcf2, &pcf3), gBreaker6(&rtc, &alarmDisps, pemsCB6Alarm1, pwm2, RGB6, P6, P7, &pcf2, &pcf3);

Generator gGenerator1(&rtc, &alarmDisps, pemsDG1Alarm1, pemsDG1Alarm2,  &display8, P0, P0, &pcf1, &pcf3), gGenerator2(&rtc, &alarmDisps, pemsDG2Alarm1, pemsDG2Alarm2, &display9, P2, P1, &pcf1, &pcf3);
Damper gDamper1(&rtc, &alarmDisps, hvacD1Alarm1, pwm3, RGB11, P0, P4, &pcf6, &pcf5), gDamper2(&rtc, &alarmDisps, hvacD2Alarm1, pwm3, RGB14, P2, P5, &pcf6, &pcf5);
ValveLinear gValve3(&rtc, &alarmDisps, hvacV3Alarm1, pwm3, RGB12, P4, &pcf6, P6, &pcf5);
Fan gFan1(&rtc, &alarmDisps, pemsCB1Alarm1, pwm3, RGB13, P6, &pcf6, P7, &pcf5);


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


//modbus
//class ModbusEthernet : public ModbusAPI<ModbusTCPTemplate<EthernetServer, EthernetClient>> {};
//IPAddress server(169, 254, 198, 12);  // Address of Modbus Slave device - need to define!!

const int32_t showDelay = 1000;   // Show result every n'th mellisecond
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEE }; // MAC address for your controller
//IPAddress ip(169, 254, 198, 201); // The IP address will be dependent on your local network
IPAddress ip(169, 254, 29, 100); // The IP address will be dependent on your local network
ModbusEthernet mb;               // Declare ModbusTCP instance
boolean mbOn = true;

void setup()
{

	Serial.begin(9600);
	delay(1000);

  pcfAllInInit(pcf1);
  pcfAllInInit(pcf2);
  pcfAllInInit(pcf3);
  pcfAllInInit(pcf4);
  pcfAllInInit(pcf5);
  pcfAllInInit(pcf6);
	
  pwmInit(pwm1);
  pwmInit(pwm2);
  pwmInit(pwm3);


  delay(1000);

  
  dispInit(display1, true);
  dispInit(display2, false);
  dispInit(display3, false);
  dispInit(display4, false);
  dispInit(display5, false);
  dispInit(display6, false);
  dispInit(display7, false);
  dispInit(display8, false);
  dispInit(display9, false);
  dispInit(display10, false);
  dispInit(display11, false);
  dispInit(display12, false);
  dispInit(display13, false);
  dispInit(display14, false);
  dispInit(display15, false);

  delay(1000);

  //rotate by 180 deg
  display1.setRotation(2);
  display2.setRotation(2);
  display3.setRotation(2);
  display4.setRotation(2);

  display5.setRotation(2);
  display6.setRotation(2);
  display7.setRotation(2);

  display12.setRotation(2);


  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display1.display();
  display2.display();
  display3.display();
  display4.display();
  display5.display();
  display6.display();
  display7.display();
  display8.display();
  display9.display();
  display10.display();
  display11.display();
  display12.display();
  display13.display();
  display14.display();
  display15.display();
  

  // Clear the buffer
  display1.clearDisplay();
  display2.clearDisplay();
  display3.clearDisplay();
  display4.clearDisplay();
  display5.clearDisplay();
  display6.clearDisplay();
  display7.clearDisplay();
  display8.clearDisplay();
  display9.clearDisplay();
  display10.clearDisplay();
  display11.clearDisplay();
  display12.clearDisplay();
  display13.clearDisplay();
  display14.clearDisplay();
  display15.clearDisplay();

  

  delay(1000);
  W5500Reset(); //needed?

  //rtc
  if (! rtc.begin()) 
  {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  if (setTime) 
  {
    Serial.println("RTC lost power, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }


  if(mbOn)
  {
    Serial.print("Starting modbus tcp ... ");
    //Modbus
    Ethernet.init(34);
    Ethernet.begin(mac, ip);
    Serial.print("CLient IP adress: ");
    Serial.println(Ethernet.localIP());

    delay(1000); //second to initialize

    uint16_t numOfTries = 0;

    // Make sure the physical link is up before continuing.
    while (Ethernet.linkStatus() == LinkOFF) {
        Serial.println("The Ethernet cable is unplugged...");
        delay(1000);
        numOfTries++;
        if(numOfTries > 5)
          break;
    }
    Serial.println("The Ethernet cable is connected.");
    Serial.println("Client connecting to Server");
    mb.connect(server, PORT);
    delay(1000);
    
    numOfTries = 0;
    while(1)
    {
      if (mb.isConnected(server))
      {
        Serial.println("Successfully connected"); 
        break;  
      }
      else
      {
        Serial.println("Connection failed, attempting to reconnect.");
        mb.connect(server, PORT);
        numOfTries++;
      }
      if(numOfTries > 5)
      {
        mbOn = false;
        break;
      }
        
    }
    if(mbOn)
      mb.client();
  }

  //joystick test

  gPump1.nomPressure = 12.0;
  gPump1.maxPressure = 13.5;
  gPump1.maxInflow = 160;
  gPump1.actInflow = 0;
  gPump1.maxSpeed = 900;
  gPump1.refSpeed = 900;
  gPump1.speed = 0;
  gPump2.maxPressure = 13.5;
  gPump2.nomPressure = 12.0;
  gPump2.maxInflow = 160;
  gPump2.actInflow = 0;
  gPump2.maxSpeed = 900;

  //pemsCB1Alarm1.time = pemsCB2Alarm1.time = pemsCB3Alarm1.time = pemsCB4Alarm1.time = pemsCB5Alarm1.time = pemsCB6Alarm1.time = rtcTime2String(rtc);
  //vmsP1Alarm1.time = rtcTime2String(rtc);
  

  display1.display();
  display2.display();
  display3.display();
  display4.display();
  display15.display();

}
  

boolean test1 = 0;
uint16_t test2 = 0;
uint16_t test3 = 0;
uint16_t test4 = 0;
bool boolWrite = false;
uint16_t progress = 0;
uint16_t progressDG1 = 0;
uint16_t progressDG2 = 0;
uint8_t state = 0;
uint32_t showLast = 0;


int dispRefreshTime = 500;
unsigned long timeNow = 0;

bool simulationLoop = true;
bool simulationInit = true;
unsigned long simTimer = 0;
enum simStateEnum{Init, fBreakersCloseCmd, fBreakersClosing, fGenStartCmd, fGenStarting, fGenRunning5s, cb5TripCmd, cb5Tripped, cb5Tripped5s, fGenRst, fGenRst5s,
                  cb5CloseCmd, cb5Closing, fGenStartCmd2, fGenStarting2, fGenRunning5s2, setAzipodRpm, shipAccelerating, shipOnRefRpm, shipTurning, shipOnRefAngle,
                  fireAlarmAct, valve2OpenCmd, pump2Starting, pump2Running10s, pump1Starting, pump1SlowingDown, pump1Stopping, v3On, v3Cooling, Restart};
simStateEnum simState = Init;

void loop()
{

  //printAlarmIndex();

 
  /*
  1. READ

  
  2. SIMULATE
  3. WRITE
  4. VISUALIZE
  5. SAVE PREV STATE
  */

  if(!simulationLoop)
  {
    // 1. READ
    //---------- VMS -----------
    gValve1.readMode();
    gValve1.readState();
    gValve2.readMode();
    gValve2.readState();
    gPump1.readMode();
    gPump1.readState();
    gPump2.readMode();
    gPump2.readState();

    //---------- PEMS -----------
    gBreaker1.readMode();
    gBreaker1.readState(mb, Brkr1CmdClsAut_ADR);
    gBreaker2.readMode();
    //TODO add adresses
    gBreaker2.readState(mb, Brkr2CmdClsAut_ADR);
    gBreaker3.readMode();
    gBreaker3.readState(mb, Brkr3CmdClsAut_ADR);
    gBreaker4.readMode();
    gBreaker4.readState(mb, Brkr4CmdClsAut_ADR);
    gBreaker5.readMode();
    gBreaker5.readState(mb, Brkr5CmdClsAut_ADR);
    gBreaker6.readMode();
    gBreaker6.readState(mb, Brkr6CmdClsAut_ADR);

    gGenerator1.readMode();
    gGenerator1.readBreakersState(gBreaker1.breakerState == eBreakerState::Closed, gBreaker5.breakerState == eBreakerState::Closed);
    gGenerator1.readState();
    
    gGenerator2.readMode();
    gGenerator2.readBreakersState(gBreaker2.breakerState == eBreakerState::Closed, gBreaker6.breakerState == eBreakerState::Closed);
    gGenerator2.readState();

    grcsVars.actPower = gGenerator1.power/1000.0 + gGenerator2.power/1000.0;   //MW
    grcsVars.actPowerBT = gGenerator1.power/1000.0 + gGenerator2.power/1000.0;   //MW



    //RCS
    rcsAzipodReadData(grcsVars, task);
    rcsBowThrustersReadData(grcsVars, task);

    //HVAC
    gDamper1.readMode();
    gDamper1.readState();
    gDamper2.readMode();
    gDamper2.readState();
    gValve3.readMode();
    gValve3.readState();
    gFan1.readMode();
    gFan1.readState();
  }
 



  //Serial.println(gValve1.valveState);

  // 2. SIMULATE
  gFan1.readMode();

  /*
  if(gFan1.fanMode == Auto)
    simulationLoop = false;
  else if(gFan1.fanMode == Local && !simulationLoop)
  {
    simulationLoop = true;
    simState = Init;
  }\
  */
  simulationLoop = false;

  //Simulation loop for demonstration
  if(simulationLoop)
  {
    switch (simState)
    {
    case Init:
      gBreaker1.breakerState = eBreakerState::Opened;
      gBreaker2.breakerState = eBreakerState::Opened;
      gBreaker3.breakerState = eBreakerState::Opened;
      gBreaker4.breakerState = eBreakerState::Opened;
      gBreaker5.breakerState = eBreakerState::Opened;
      gBreaker6.breakerState = eBreakerState::Opened;

      gBreaker1.writeCmd();
      gBreaker2.writeCmd();
      gBreaker3.writeCmd();
      gBreaker4.writeCmd();
      gBreaker5.writeCmd();
      gBreaker6.writeCmd();
      delay(5000);
      simState = fBreakersCloseCmd;
      break;
    
    case fBreakersCloseCmd:
      gBreaker1.timer = millis();
      gBreaker1.breakerState = eBreakerState::Closing;
      gBreaker5.timer = millis();
      gBreaker5.breakerState = eBreakerState::Closing;

      gFan1.fanState = eFanState::Starting;
      gDamper1.damperState = eDamperState::Opening;
      gDamper2.damperState = eDamperState::Opening;

      simState = fBreakersClosing;
      break;
    
    case fBreakersClosing:
      if(gBreaker1.breakerState == eBreakerState::Closed && gBreaker5.breakerState == eBreakerState::Closed)
        simState = fGenStartCmd;
      break;
    
    case fGenStartCmd:
      gGenerator1.nomPower = 500.0;
      gGenerator1.generatorState = Starting;
      simTimer = millis();
      simState = fGenStarting;
      break;  

    case fGenStarting:
      if(gGenerator1.generatorState == Running)
      {
        simTimer = millis();
        simState = fGenRunning5s;
      }
        
      break;
    
    case fGenRunning5s:
        if(millis() - simTimer > 5000)
          simState = cb5TripCmd;
      break;  
    
    case cb5TripCmd:
      gBreaker5.timer = millis();
      gBreaker5.breakerState = eBreakerState::Opening;
      gGenerator1.failure = true;
      gGenerator1.generatorState = StoppingF;
      simState = cb5Tripped;
      break;
    
    case cb5Tripped:
      if(gGenerator1.power == 0.0)
      {
        gGenerator1.generatorState = Failure2;
        gGenerator1.generatorAlarm2.time = rtcTime2String(rtc);
        dispShowAlarm(*gGenerator1.alarmDisps->d1, *gGenerator1.alarmDisps->d2, *gGenerator1.alarmDisps->d3, *gGenerator1.alarmDisps->d4, gGenerator1.generatorAlarm2, 1);
        simState = cb5Tripped5s;
        simTimer = millis();
      }      
      break;
    
    case cb5Tripped5s:
      if(TOff(5000, &simTimer))
        simState = fGenRst;
      break;
    
    case fGenRst:
      gGenerator1.generatorState = Stopped;
      dispClearAlarms(display1, display2, display3, display4);
      

      gBreaker6.timer = millis();
      gBreaker6.breakerState = eBreakerState::Closing;
      gBreaker2.timer = millis();
      gBreaker2.breakerState = eBreakerState::Closing;

      gGenerator2.nomPower = 500.0;
      gGenerator2.timer = millis();
      gGenerator2.generatorState = Starting;
      simTimer = millis();
      simState = fGenRst5s;
      break;
    
    case fGenRst5s:
      if(TOff(5000, &simTimer))
        simState = cb5CloseCmd;
      break;
    
    case cb5CloseCmd:
      gBreaker5.timer = millis();
      gBreaker5.breakerState = eBreakerState::Closing;
      simState = cb5Closing;
      break;

    case cb5Closing:
      if(gBreaker5.breakerState == eBreakerState::Closed)
      {
        gGenerator1.nomPower = 2000;
        gGenerator1.generatorState = Starting;
        simState = fGenStarting2;
      }
      break;
    
    case fGenStarting2:
      if(gGenerator1.generatorState == Running)
      {
        simTimer = millis();
        simState = fGenRunning5s2;
      }
      break;
    
     case fGenRunning5s2:
        if(TOff(5000, &simTimer))
          simState = setAzipodRpm;        
        break;
      
      case setAzipodRpm:
        grcsVars.refRPM = 150;       
        simState = shipAccelerating;       
        break;

      case shipAccelerating:
        
        if(grcsVars.actRPM >= 150.0)
        {
          simState = shipOnRefRpm;
          simTimer = millis();
        }
        break;
          
      case shipOnRefRpm:
        if(TOff(5000, &simTimer))
        {
          grcsVars.refAnglePORT = 20.0;
          simState = shipTurning;
        }
        break;
      
      case shipTurning:
        if(grcsVars.actAnglePORT >= 20.0)
        {
          simState = shipOnRefAngle;
          simTimer = millis();
        }
          
        break;
      
      case shipOnRefAngle:
        if(TOff(5000, &simTimer))
          gDamper1.timer = millis();
          gDamper2.timer = millis();
          simState = fireAlarmAct;
        break;
      
      case fireAlarmAct:
        //HVAC
        gDamper1.timer = millis();
        gDamper2.timer = millis();
        gDamper1.damperState = eDamperState::Closing;
        gDamper2.damperState = eDamperState::Closing;
        //VMS
        fireAlarm.time = rtcTime2String(rtc);
        dispShowAlarm(*gGenerator1.alarmDisps->d1, *gGenerator1.alarmDisps->d2, *gGenerator1.alarmDisps->d3, *gGenerator1.alarmDisps->d4, fireAlarm, 1);
        gValve2.valveState = Opening;
        simState = valve2OpenCmd;
        break;
      
      case valve2OpenCmd:
        if(gValve2.valveState == Opened)
        {
          gPump2.pumpState = Starting;
          simState = pump2Starting;
          simTimer = millis();
        }
        break;
      
       case pump2Starting:
          if(TOff(10000, &simTimer))
          {
            gPump1.pumpState = Starting;
            gPump1.refSpeed = 950;
            gValve1.valveState = Opening;
            simState = pump1Starting;
          }
          break;

      case pump1Starting:
          if(gVmsSimVars.PressureAct > 5.0 && gValve2.valveState != Closed)
          {
            gValve2.timer = millis();
            gValve2.valveState = Closed;
            dispClearAlarms(display1, display2, display3, display4);
            //HVAC
            gDamper1.timer = millis();
            gDamper2.timer = millis();
            gDamper1.damperState = eDamperState::Opening;
            gDamper2.damperState = eDamperState::Opening;
          }
          if(gVmsSimVars.PressureAct >= 7.2)
          {
            gPump1.pumpState = Stopping;
            gValve1.valveState = Closing;
            simState = pump1Stopping;
            gPump2.pumpState = Stopping;
            simState = pump1Stopping;
          }
        break;
      
      
      case pump1Stopping:

        if(gPump1.pumpState == Stopped)
        {
          simTimer = millis();
          simState = v3On;
        }
        break;

      case v3On:
        if(TOff(3000, &simTimer))
        {
          gValve3.timer = millis();
          gValve3.valveState = eValveLinState::Opening;
          gHvacSimVars.tempRef = 19.5;
          simTimer = millis();
          simState = v3Cooling;
        }

        break;
      
      case v3Cooling:
        if(gHvacSimVars.temp <= 20.0)
        {
          gValve3.timer = millis();
          gValve3.valveState = eValveLinState::Closing;
          simTimer = millis();
          simState = Restart;
        }
        
        break;

      case Restart:
          if(TOff(5000, &simTimer))
          {
            ESP.restart();
          }
        
        break;


    default:
      break;
    }

    grcsVars.actPower = gGenerator1.power/1000.0 + gGenerator2.power/1000.0;   //MW
    grcsVars.actPowerBT = gGenerator1.power/1000.0 + gGenerator2.power/1000.0;   //MW

  }


 
    

/*shipOnRefAngle,
                  fireAlarm, valve2OpenCmd*/

    






    
  


  //---------- VMS -----------
  vmsSimluation(gPump1, gPump2, gValve1, gValve2, gVmsSimVars, task);

  //---------- HVAC -----------
  hvacSimulation(gDamper1, gDamper2, gValve3, gFan1, gHvacSimVars);

  //---------- RCS ------------
  rcsAzipodSimulate(grcsVars);



  //3. WRITE
  //---------- VMS -----------
  gValve1.writeCmd();
  gValve2.writeCmd();
  gPump1.writeCmd();
  gPump2.writeCmd();

  //---------- PEMS -----------
  gBreaker1.writeCmd();
  gBreaker2.writeCmd();
  gBreaker3.writeCmd();
  gBreaker4.writeCmd();
  gBreaker5.writeCmd();
  gBreaker6.writeCmd();

  gGenerator1.writeCmd();
  gGenerator2.writeCmd();

  //---------- HVAC -----------
  if(mbOn)
    hvacWrite(mb, gHvacSimVars);
  gDamper1.writeCmd();
  gDamper2.writeCmd();
  gValve3.writeCmd();
  gFan1.writeCmd();

  // 4. VISUALIZE
 
  if(millis() - timeNow > dispRefreshTime)
  {
    //---------- VMS -----------
    vmsDispPump(display10, gPump1.speed, gPump1.pressure, gPump2.pressure);
    vmsDispPressure(display11, gVmsSimVars.PressureRef, gVmsSimVars.PressureAct);

    //---------- HVAC -----------
    hvacVisualization(display12, gFan1, gHvacSimVars);
    timeNow = millis();

  }

  //---------- PEMS -----------
  gGenerator1.visualize();
  gGenerator2.visualize();

  //---------- RCS -----------
  dispRCSAzipodVisualize(display5, display6, display7, grcsVars);
  dispRCSBowThrustersVisualize(display13, display14, display15, grcsVars);


  // 5. SAVE PREV STATE
  //---------- VMS -----------
  gPump1.savePrevState();
  gPump2.savePrevState();
  gValve1.savePrevState();
  gValve2.savePrevState();

  //---------- PEMS -----------
  gBreaker1.savePrevState();
  gBreaker2.savePrevState();
  gBreaker3.savePrevState();
  gBreaker4.savePrevState();
  gBreaker5.savePrevState();
  gBreaker6.savePrevState();

  gGenerator1.savePrevState();
  gGenerator2.savePrevState();

  //---------- HVAC -----------
  gDamper1.savePrevState();
  gDamper2.savePrevState();
  gValve3.savePrevState();
  gFan1.savePrevState();

  //6. DEBUG

  //Serial.println(gPump1.timer);
 // Serial.println(gPump2.timer);
 
 /*
  String voltageX1 = "X1 " + String(analogReadMilliVolts(JOY1_X)) + "mV";
  String voltageY1 = "Y1 " + String(analogReadMilliVolts(JOY1_Y)) + "mV";
  String voltageX2 = "X2 " + String(analogReadMilliVolts(JOY2_X)) + "mV";
  String voltageY2 = "Y2 " + String(analogReadMilliVolts(JOY2_Y)) + "mV";
  display15.clearDisplay();
  dispStringALigned(voltageX1, display15, DejaVu_Sans_Mono_10, LeftTop, 0, 0);
  dispStringALigned(voltageY1, display15, DejaVu_Sans_Mono_10, LeftTop, 0, 10);
  dispStringALigned(voltageX2, display15, DejaVu_Sans_Mono_10, LeftTop, 0, 20);
  dispStringALigned(voltageY2, display15, DejaVu_Sans_Mono_10, LeftTop, 0, 30);
  display15.display();*/

 
  delay(task);

  /*RGBLedTest(5, pwm1);
  RGBLedTest(5, pwm2);
  RGBLedTest(4, pwm3);*/

  //Serial.println("Alarm removed" + String(alarmRemoved));
  //Serial.println("Alarm added" + String(newAlarmAdded));
 
  



  boolWrite = !boolWrite;

  //resetAlarmIndex();
  //newAlarmAdded = false;
 
  
  

  if(mbOn)
  {
    Serial.println(gBreaker1.breakerMode);
    
    if(Ethernet.linkStatus() == LinkOFF) 
    {
      Serial.println("The Ethernet cable is unplugged...");
    }
    else
    {
       if (mb.isConnected(server)) 
      {  
          mb.readCoil(server, 1, &test1);
          mb.readHreg(server, 1, &test2); 
          mb.readHreg(server, 2, &test3); 
          mb.readHreg(server, 100, &test4); 
          mb.writeCoil(server, 1, boolWrite);
      } 
      else 
      {
          mb.connect(server);           // Try to connect if not connected
      }
      //delay(100);                     // Pulling interval
                          // Common local Modbus task
      if (millis() - showLast > showDelay)
      { // Display register value every x seconds (with default settings)
        mb.task(); 
        showLast = millis();
        Serial.print("Test1: ");
        Serial.println(test1);
        Serial.print("Test2: ");
        Serial.println(test2);
        Serial.print("Test3: ");
        Serial.println(test3);
        Serial.print("Test4: ");
        Serial.println(test4);
      }

    }

   
  }
  
}


