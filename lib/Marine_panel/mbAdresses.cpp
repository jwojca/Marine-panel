#include "mbAdresses.h"

IPAddress server(172, 16, 80, 1);  // Address of Modbus Slave device - need to define!!
uint16_t mbReadDelay = 300;

const uint16_t HregsCount = 30;
const uint16_t coilsCount = 200;


bool arrayCoilsR[coilsCount];
bool arrayCoilsW[coilsCount];
uint16_t arrayHregsR[HregsCount];
uint16_t arrayHregsW[HregsCount];

uint16_t coilsWrOffset = 200;
uint16_t HregsWrOffset = 30;

uint16_t mbTaskDelay = 2;


void readBools(ModbusEthernet &mb)
{
    uint16_t transRead = mb.readCoil(server, 0, arrayCoilsR, coilsCount);
    while(mb.isTransaction(transRead))
    {
        mb.task();
        //Serial.println("Reading Coils");
        delay(mbTaskDelay);
    }
}

void writeBools(ModbusEthernet &mb)
{
    uint16_t transWrite = mb.writeCoil(server, coilsWrOffset, arrayCoilsW, coilsCount);
    while(mb.isTransaction(transWrite))
    {
        mb.task();
        //Serial.println("Writing Coils");
        delay(mbTaskDelay);
    }
}
void readInts(ModbusEthernet &mb)
{
    uint16_t transReadH = mb.readHreg(server, 0, arrayHregsR, HregsCount);
    while(mb.isTransaction(transReadH))
    {
        mb.task();
        //Serial.println("Reading Hregs");
        delay(mbTaskDelay);
    }

}
void writeInts(ModbusEthernet &mb)
{
    uint16_t transWriteH = mb.writeHreg(server, HregsWrOffset, arrayHregsW, HregsCount);
    while(mb.isTransaction(transWriteH))
    {
        mb.task();
        //Serial.println("Writing Hregs");
        delay(mbTaskDelay);
    }

}

void initRegs()
{
    for(uint16_t i = 0; i < HregsCount; ++i)
    {
        arrayHregsR[i] = 0;
        arrayHregsW[i] = 0;
    }

    for(uint16_t i = 0; i < coilsCount; ++i)
    {
        arrayCoilsR[i] = false;
        arrayCoilsW[i] = false;
    }
}
