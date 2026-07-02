#include "rscan_mx_v3_client.h"
#include <string.h>
#include <iostream>

RSCANMXV3Client::RSCANMXV3Client(std::string ip, uint16_t port)
    : TCPClient(ip, port, kRxBufferSize, kTxBufferSize)
{
    // value sizes and offsets as it is in MCU program
    map = std::vector<ValueAddress>(RSCANMXV3ValueCode::Size);
    map[RSCANMXV3ValueCode::Message_Num]   = {0, 4};
    map[RSCANMXV3ValueCode::HV]            = {4, 2};
    map[RSCANMXV3ValueCode::Data]          = {64, 3362};
}

RSCANMXV3Client::~RSCANMXV3Client()
{}

int RSCANMXV3Client::getMessageNum()
{
    int res = 0;
    TCPClient::copyFromRx(&res, map[RSCANMXV3ValueCode::Message_Num].offset, map[RSCANMXV3ValueCode::Message_Num].size);
    return res;
}

uint16_t RSCANMXV3Client::getHV()
{
    uint16_t res = 0;
    TCPClient::copyFromRx(&res, map[RSCANMXV3ValueCode::HV].offset, map[RSCANMXV3ValueCode::HV].size);
    return res;
}

int RSCANMXV3Client::getCellSignal(int cellNo)
{
    int res = 0;
    TCPClient::copyFromRx(&res, map[RSCANMXV3ValueCode::Data].offset + 2 * cellNo, 2);
    return res;
}
