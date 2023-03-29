#ifndef ALARMS_H
#define ALARMS_H

#include <Arduino.h>

struct mpAlarm
{
  String time;
  String objName;
  String eventKind;
  String descr;
};

//----------------------------------------- VMS -----------------------------------------------------
static mpAlarm vmsP1Alarm1 {"26-10-2022 08:47", "VMS Pump 1", "Critical", "Power failure"};
static mpAlarm vmsP2Alarm1 {"26-10-2022 08:47", "VMS Pump 2", "Low priority", "Bla bla"};
static mpAlarm vmsV1Alarm1 {"26-10-2022 08:47", "VMS Valve 1", "Critical", "Not opening"};
static mpAlarm vmsV2Alarm1 {"26-10-2022 08:47", "VMS Valve 2", "Low priority", "Clogged"};


//----------------------------------------- PEMS -----------------------------------------------------
static mpAlarm pemsDG1Alarm1 {"26-10-2022 08:47", "PEMS Generator 1", "Med priority", "Connect fail"};
static mpAlarm pemsDG1Alarm2 {"26-10-2022 08:47", "PEMS Generator 1", "Med priority", "Tripped"};
static mpAlarm pemsDG2Alarm1 {"26-10-2022 08:47", "PEMS Generator 2", "Med priority", "Connect fail"};
static mpAlarm pemsDG2Alarm2 {"26-10-2022 08:47", "PEMS Generator 2", "Med priority", "Tripped"};

static mpAlarm pemsCB1Alarm1 {"26-10-2022 08:47", "PEMS Cbreaker 1", "Critical", "Bla bla"};
static mpAlarm pemsCB2Alarm1 {"26-10-2022 08:47", "PEMS Cbreaker 2", "Low priority", "Bla bla"};
static mpAlarm pemsCB3Alarm1 {"26-10-2022 08:47", "PEMS Cbreaker 3", "Critical", "Bla bla"};
static mpAlarm pemsCB4Alarm1 {"26-10-2022 08:47", "PEMS Cbreaker 4", "Low priority", "Bla bla"};
static mpAlarm pemsCB5Alarm1 {"26-10-2022 08:47", "PEMS Cbreaker 5", "Critical", "Bla bla"};
static mpAlarm pemsCB6Alarm1 {"26-10-2022 08:47", "PEMS Cbreaker 6", "Low priority", "Bla bla"};

//----------------------------------------- HVAC -----------------------------------------------------
static mpAlarm hvacD1Alarm1 {"26-10-2022 08:47", "HVAC Damper 1", "Med priority", "Connect fail"};
static mpAlarm hvacD2Alarm1 {"26-10-2022 08:47", "HVAC Damper 1", "Med priority", "Tripped"};
static mpAlarm hvacV3Alarm1 {"26-10-2022 08:47", "HVAC Valve 3", "Med priority", "Connect fail"};
static mpAlarm hvacF1Alarm1 {"26-10-2022 08:47", "HVAC Fan 2", "Med priority", "Tripped"};

//------------------------------------Fire alarm -----------------------------------------------------
static mpAlarm fireAlarm {"26-10-2022 08:47", "Fire alarm", "High priority", ""};


#endif