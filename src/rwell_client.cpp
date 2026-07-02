#include "rwell_client.h"
#include <string.h>
#include <iostream>

RWELLClient::RWELLClient(std::string ip, uint16_t port)
    : TCPClient(ip, port, kRxBufferSize, kTxBufferSize)
{
    map = std::vector<ValueAddress>(RWELLValueCode::Size);
    map[RWELLValueCode::Message_Num]   = {0, 4};
    map[RWELLValueCode::ADC_DR]        = {4, 4};
    map[RWELLValueCode::HV_Out]        = {8, 2};
    map[RWELLValueCode::Pressure]      = {10, 4};
    map[RWELLValueCode::Temperature]   = {14, 4};
    map[RWELLValueCode::Band]          = {18, 1};
}

RWELLClient::~RWELLClient()
{}

int RWELLClient::getMessageNum()
{
    int res = 0;
    TCPClient::copyFromRx(
        &res,
        map[RWELLValueCode::Message_Num].offset,
        map[RWELLValueCode::Message_Num].size
        );
    return res;
}
int RWELLClient::getADCValue()
{
    int res = 0;
    TCPClient::copyFromRx(
        &res,
        map[RWELLValueCode::ADC_DR].offset,
        map[RWELLValueCode::ADC_DR].size
    );
    return res;

}
int RWELLClient::getHV()
{
    int res = 0;
    TCPClient::copyFromRx(
        &res,
        map[RWELLValueCode::HV_Out].offset,
        map[RWELLValueCode::HV_Out].size
    );
    return res;
}
int RWELLClient::getPressure()
{
    int res = 0;
    TCPClient::copyFromRx(
        &res,
        map[RWELLValueCode::Pressure].offset,
        map[RWELLValueCode::Pressure].size
    );
    return res;    

}
int RWELLClient::getTemperature()
{
    int res = 0;
    TCPClient::copyFromRx(
        &res,
        map[RWELLValueCode::Temperature].offset,
        map[RWELLValueCode::Temperature].size
    );
    return res; 
}
int RWELLClient::getBand()
{
    int res = 0;
    TCPClient::copyFromRx(
        &res,
        map[RWELLValueCode::Band].offset,
        map[RWELLValueCode::Band].size
    );
    return res; 

}
void RWELLClient::setHV(uint16_t volt)
{
    setCommand(RWELLCommandCode::Set_HV, volt, 0);
}
void RWELLClient::setBand(uint8_t band)
{
    setCommand(RWELLCommandCode::Set_Band, band, 0);
}
void RWELLClient::setCommand(int code, int param_1, int param_2)
{
    memset(tx, 0, kTxBufferSize);
    memcpy(tx + 0, &code, 4);
    memcpy(tx + 4, &param_1, 4);
    memcpy(tx + 8, &param_2, 4);
    TCPClient::copyToTx(tx, 0, kTxBufferSize);

}
