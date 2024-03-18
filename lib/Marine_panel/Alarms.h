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
static mpAlarm vmsP2Alarm1 {"26-10-2022 08:47", "VMS Pump 2", "Critical", "Power failure"};
static mpAlarm vmsV1Alarm1 {"26-10-2022 08:47", "VMS Valve 1", "Low priority", "Feedback fail"};
static mpAlarm vmsV2Alarm1 {"26-10-2022 08:47", "VMS Valve 2", "Low priority", "Feedback fail"};


//----------------------------------------- PEMS -----------------------------------------------------
static mpAlarm pemsDG1Alarm1 {"26-10-2022 08:47", "PEMS Generator 1", "Med priority", "Connect fail"};
static mpAlarm pemsDG1Alarm2 {"26-10-2022 08:47", "PEMS Generator 1", "Med priority", "Start fail"};
static mpAlarm pemsDG2Alarm1 {"26-10-2022 08:47", "PEMS Generator 2", "Med priority", "Connect fail"};
static mpAlarm pemsDG2Alarm2 {"26-10-2022 08:47", "PEMS Generator 2", "Med priority", "Start fail"};

static mpAlarm pemsCB1Alarm1 {"26-10-2022 08:47", "PEMS Cbreaker 1", "Critical", "Tripped"};
static mpAlarm pemsCB2Alarm1 {"26-10-2022 08:47", "PEMS Cbreaker 2", "Low priority", "Close fail"};
static mpAlarm pemsCB3Alarm1 {"26-10-2022 08:47", "PEMS Cbreaker 3", "Critical", "Tripped"};
static mpAlarm pemsCB4Alarm1 {"26-10-2022 08:47", "PEMS Cbreaker 4", "Low priority", "Close fail"};
static mpAlarm pemsCB5Alarm1 {"26-10-2022 08:47", "PEMS Cbreaker 5", "Critical", "Tripped"};
static mpAlarm pemsCB6Alarm1 {"26-10-2022 08:47", "PEMS Cbreaker 6", "Low priority", "Close fail"};

//----------------------------------------- HVAC -----------------------------------------------------
static mpAlarm hvacD1Alarm1 {"26-10-2022 08:47", "HVAC Damper 1", "Med priority", "General alarm"};
static mpAlarm hvacD2Alarm1 {"26-10-2022 08:47", "HVAC Damper 1", "Med priority", "General alarm"};
static mpAlarm hvacV3Alarm1 {"26-10-2022 08:47", "HVAC Valve 3", "Med priority", "Feedback fail"};
static mpAlarm hvacF1Alarm1 {"26-10-2022 08:47", "HVAC Fan 2", "Med priority", "Feedback fail"};

//------------------------------------Fire alarm -----------------------------------------------------
static mpAlarm fireAlarm {"26-10-2022 08:47", "Fire alarm", "High priority", ""};


#endif