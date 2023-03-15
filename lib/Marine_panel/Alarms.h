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


static mpAlarm vmsP1Alarm1 {"26-10-2022 08:47", "VMS Pump 1", "Critical", "Power failure"};
static mpAlarm vmsP2Alarm1 {"26-10-2022 08:47", "VMS Pump 2", "Low priority", "Bla bla"};
static mpAlarm vmsV1Alarm1 {"26-10-2022 08:47", "VMS Valve 1", "Critical", "Not opening"};
static mpAlarm vmsV2Alarm1 {"26-10-2022 08:47", "VMS Valve 2", "Low priority", "Clogged"};
                              
static mpAlarm pemsCB1Alarm1 {"26-10-2022 08:47", "PEMS Cbreaker 1", "Critical", "Bla bla"};
static mpAlarm pemsCB2Alarm1 {"26-10-2022 08:47", "PEMS Cbreaker 2", "Low priority", "Bla bla"};
static mpAlarm pemsCB3Alarm1 {"26-10-2022 08:47", "PEMS Cbreaker 3", "Critical", "Bla bla"};
static mpAlarm pemsCB4Alarm1 {"26-10-2022 08:47", "PEMS Cbreaker 4", "Low priority", "Bla bla"};
static mpAlarm pemsCB5Alarm1 {"26-10-2022 08:47", "PEMS Cbreaker 5", "Critical", "Bla bla"};
static mpAlarm pemsCB6Alarm1 {"26-10-2022 08:47", "PEMS Cbreaker 6", "Low priority", "Bla bla"};


#endif