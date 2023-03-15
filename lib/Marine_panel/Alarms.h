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

#endif