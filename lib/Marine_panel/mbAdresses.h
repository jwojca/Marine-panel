#pragma once

#include <Ethernet.h>       // Ethernet library v2 is required
#include <ModbusAPI.h>
#include <ModbusTCPTemplate.h>
extern IPAddress server;  // Address of Modbus Slave device - need to define!!
class ModbusEthernet : public ModbusAPI<ModbusTCPTemplate<EthernetServer, EthernetClient>> {};

extern uint16_t mbReadDelay;

extern bool arrayCoilsR[];
extern bool arrayCoilsW[];
extern uint16_t arrayHregsR[];
extern uint16_t arrayHregsW[];

extern uint16_t coilsWrOffset;
extern uint16_t HregsWrOffset;

extern float mbMultFactor;

//IO list revision C
#define Gen1StartAuto_ADR 0
#define Gen1StopAuto_ADR 1
#define Gen1Running_ADR 200
#define Gen1stopped_ADR 201
#define Gen1Local_ADR 202
#define Gen1Auto_ADR 203
#define Gen1Failure_ADR 204
#define Gen1RefPower_ADR 0
#define Gen1ActPower_ADR 30
#define Gen1ActRPM_ADR 31
#define Gen2StartAuto_ADR 2
#define Gen2StopAuto_ADR 3
#define Gen2Running_ADR 205
#define Gen2stopped_ADR 206
#define Gen2Local_ADR 207
#define Gen2Auto_ADR 208
#define Gen2Failure_ADR 209
#define Gen2RefPower_ADR 1
#define Gen2ActPower_ADR 32
#define Gen2ActRPM_ADR 33
#define Brkr1CmdClsAut_ADR 4
#define Brkr1CmdOpAut_ADR 5
#define Brkr1Closed_ADR 210
#define Brkr1Opened_ADR 211
#define Brkr1Local_ADR 212
#define Brkr1Auto_ADR 213
#define Brkr1Failure_ADR 214
#define Brkr2CmdClsAut_ADR 6
#define Brkr2CmdOpAut_ADR 7
#define Brkr2Closed_ADR 215
#define Brkr2Opened_ADR 216
#define Brkr2Local_ADR 217
#define Brkr2Auto_ADR 218
#define Brkr2Failure_ADR 219
#define Brkr3CmdClsAut_ADR 8
#define Brkr3CmdOpAut_ADR 9
#define Brkr3Closed_ADR 220
#define Brkr3Opened_ADR 221
#define Brkr3Local_ADR 222
#define Brkr3Auto_ADR 223
#define Brkr3Failure_ADR 224
#define Brkr4CmdClsAut_ADR 10
#define Brkr4CmdOpAut_ADR 11
#define Brkr4Closed_ADR 225
#define Brkr4Opened_ADR 226
#define Brkr4Local_ADR 227
#define Brkr4Auto_ADR 228
#define Brkr4Failure_ADR 229
#define Brkr5CmdClsAut_ADR 12
#define Brkr5CmdOpAut_ADR 13
#define Brkr5Closed_ADR 230
#define Brkr5Opened_ADR 231
#define Brkr5Local_ADR 232
#define Brkr5Auto_ADR 233
#define Brkr5Failure_ADR 234
#define Brkr6CmdClsAut_ADR 14
#define Brkr6CmdOpAut_ADR 15
#define Brkr6Closed_ADR 235
#define Brkr6Opened_ADR 236
#define Brkr6Local_ADR 237
#define Brkr6Auto_ADR 238
#define Brkr6Failure_ADR 239
#define Vlv1CmdClsAut_ADR 16
#define Vlv1CmdOpAut_ADR 17
#define Vlv1Closed_ADR 240
#define Vlv1Opened_ADR 241
#define Vlv1Local_ADR 242
#define Vlv1Auto_ADR 243
#define Vlv1Failure_ADR 244
#define Vlv2CmdClsAut_ADR 18
#define Vlv2CmdOpAut_ADR 19
#define Vlv2Closed_ADR 245
#define Vlv2Opened_ADR 246
#define Vlv2Local_ADR 247
#define Vlv2Auto_ADR 248
#define Vlv2Failure_ADR 249
#define Pmp2Failure_ADR 250
#define Pmp2Pressure_ADR 34
#define Pmp2CmdStpAut_ADR 20
#define Pmp2CmdStrtAut_ADR 21
#define Pmp2Running_ADR 251
#define Pmp2Stopped_ADR 252
#define Pmp2Local_ADR 253
#define Pmp2Auto_ADR 254
#define Pmp1Failure_ADR 255
#define Pmp1Pressure_ADR 35
#define Pmp1CmdStpAut_ADR 22
#define Pmp1CmdStrtAut_ADR 23
#define Pmp1Running_ADR 256
#define Pmp1Stopped_ADR 257
#define Pmp1Local_ADR 258
#define Pmp1Auto_ADR 259
#define Pmp1SpeedRef_ADR 2
#define Pmp1SpeedAct_ADR 36
#define VmsPressRef_ADR 3
#define VmsPressAct_ADR 37
#define Dmp1OpLim_ADR 260
#define Dmp1ClLim_ADR 261
#define Dmp1CmdOpAut_ADR 24
#define Dmp2OpLim_ADR 262
#define Dmp2ClLim_ADR 263
#define Dmp2CmdOpAut_ADR 25
#define FanCmdStartAut_ADR 26
#define FanCmdStopAut_ADR 27
#define FanLocal_ADR 264
#define FanAuto_ADR 265
#define FanFailure_ADR 266
#define FanRunning_ADR 267
#define FanPosRef_ADR 4
#define FanPosAct_ADR 38
#define Vlv3CmdClsAut_ADR 28
#define Vlv3CmdOpAut_ADR 29
#define Vlv3Closed_ADR 268
#define Vlv3Opened_ADR 269
#define Vlv3Local_ADR 270
#define Vlv3Auto_ADR 271
#define Vlv3Failure_ADR 272
#define Vlv3PosRef_ADR 5
#define Vlv3PosAct_ADR 39
#define SnsrPressRef_ADR 6
#define SnsrPressAct_ADR 40
#define SnsrTempRef_ADR 7
#define SnsrTempAct_ADR 41
#define AzpdRefSpeed_ADR 8
#define AzpdRefAngSTBD_ADR 9
#define AzpdRefAngPORT_ADR 10
#define AzpdActSpeed_ADR 42
#define AzpdActAngSTBD_ADR 43
#define AzpdActAngPORT_ADR 44
#define BtrRefSpSTBD_ADR 11
#define BtrRefSpPORT_ADR 12
#define BtaActSpSTBD_ADR 45
#define BtaActSpPORT_ADR 46
#define EsdActive_ADR 273
#define FireAlarmActive_ADR 274
#define Dmp1Fail_ADR 275
#define Dmp1Aut_ADR 276
#define Dmp2Fail_ADR 277
#define Dmp2Aut_ADR 278



void readBools(ModbusEthernet &mb);
void writeBools(ModbusEthernet &mb);
void readInts(ModbusEthernet &mb);
void writeInts(ModbusEthernet &mb);
void initRegs();