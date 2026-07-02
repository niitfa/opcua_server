#include "rstep_client.h"
#include <string.h>
#include <iostream>

RSTEPClient::RSTEPClient(std::string ip, uint16_t port)
    : TCPClient(ip, port, kRxBufferSize, kTxBufferSize)
{
    map = std::vector<ValueAddress>(RSTEPValueCode::Size);
    map[RSTEPValueCode::Message_Num]   = {0, 4};
    map[RSTEPValueCode::Pos_Long]      = {4, 4};
    map[RSTEPValueCode::Pos_Ang]       = {8, 2};
}


RSTEPClient::~RSTEPClient()
{}

int RSTEPClient::getMessageNum()
{
    int res = 0;
    TCPClient::copyFromRx(
        &res,
        map[RSTEPValueCode::Message_Num].offset,
        map[RSTEPValueCode::Message_Num].size
        );
    return res;
}

int RSTEPClient::getLongPosition()
{
    int res = 0;
    TCPClient::copyFromRx(
        &res,
        map[RSTEPValueCode::Pos_Long].offset,
        map[RSTEPValueCode::Pos_Long].size
        );
    return res;
}

int RSTEPClient::getAngPosition()
{
    int res = 0;
    TCPClient::copyFromRx(
        &res,
        map[RSTEPValueCode::Pos_Ang].offset,
        map[RSTEPValueCode::Pos_Ang].size
        );
    return res;
}

void RSTEPClient::moveLong(int position, int velosity)
{
    setCommand(RSTEPCommandCode::Move_Long, position, velosity);
}

void RSTEPClient::moveAng(int position, int velosity)
{
    setCommand(RSTEPCommandCode::Move_Ang, position, velosity);
}

void RSTEPClient::stopLong()
{
    setCommand(RSTEPCommandCode::Stop_Long, 0, 0);
}

void RSTEPClient::stopAng()
{
    setCommand(RSTEPCommandCode::Stop_Ang, 0, 0);
}

void RSTEPClient::resetLong(int pos, int velosity)
{
    setCommand(RSTEPCommandCode::Reset_Long, 0, 0);
}

void RSTEPClient::resetAng(int pos, int velosity)
{
    setCommand(RSTEPCommandCode::Reset_Ang, 0, 0);
}

void RSTEPClient::setCommand(int code, int param_1, int param_2)
{
    memset(tx, 0, kTxBufferSize);
    memcpy(tx + 0, &code, 4);
    memcpy(tx + 4, &param_1, 4);
    memcpy(tx + 8, &param_2, 4);
    TCPClient::copyToTx(tx, 0, kTxBufferSize);
}



