#include "mbAdresses.h"

IPAddress server(172, 16, 80, 1);  // Address of Modbus Slave device - need to define!!
uint16_t mbReadDelay = 300;

const uint16_t HregsCount = 30;
const uint16_t coilsCount = 200;

uint16_t arrayInt[HregsCount];
bool arrayRead[coilsCount];

uint16_t mbTaskDelay = 2;


void readBools(ModbusEthernet &mb)
{
    uint16_t transRead = mb.readCoil(server, 0, arrayRead, coilsCount);
    while(mb.isTransaction(transRead))
    {
        mb.task();
        //Serial.println("Reading Coils");
        delay(mbTaskDelay);
    }
}

void writeBools(ModbusEthernet &mb)
{
    uint16_t transWrite = mb.writeCoil(server, 200, arrayRead, coilsCount);
    while(mb.isTransaction(transWrite))
    {
        mb.task();
        //Serial.println("Writing Coils");
        delay(mbTaskDelay);
    }
}
void readInts(ModbusEthernet &mb)
{
    uint16_t transReadH = mb.readHreg(server, 0, arrayInt, HregsCount);
    while(mb.isTransaction(transReadH))
    {
        mb.task();
        //Serial.println("Reading Hregs");
        delay(mbTaskDelay);
    }

}
void writeInts(ModbusEthernet &mb)
{
    uint16_t transWriteH = mb.writeHreg(server, 100, arrayInt, HregsCount);
    while(mb.isTransaction(transWriteH))
    {
        mb.task();
        //Serial.println("Writing Hregs");
        delay(mbTaskDelay);
    }

}
