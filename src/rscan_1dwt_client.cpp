#include "rscan_1dwt_client.h"
#include <string.h>
#include <iostream>

RSCAN_1DWT_Client::RSCAN_1DWT_Client(std::string ip, uint16_t port)
    : TCPClient(ip, port, kRxBufferSize, kTxBufferSize)
{
    // value sizes and offsets as it is in MCU program
    map = std::vector<ValueAddress>(RSCAN_1DWT_ValueCode::Size);
    map[RSCAN_1DWT_ValueCode::Message_Num]      = {0, 4};
    map[RSCAN_1DWT_ValueCode::Pos_Z]            = {4, 4};
}

RSCAN_1DWT_Client::~RSCAN_1DWT_Client()
{}

int RSCAN_1DWT_Client::getMessageNum()
{
    int res = 0;
    TCPClient::copyFromRx(&res, map[RSCAN_1DWT_ValueCode::Message_Num].offset, map[RSCAN_1DWT_ValueCode::Message_Num].size);
    return res;
}
int RSCAN_1DWT_Client::getZ()
{
    int res = 0;
    TCPClient::copyFromRx(&res, map[RSCAN_1DWT_ValueCode::Pos_Z].offset, map[RSCAN_1DWT_ValueCode::Pos_Z].size);
    return res;
}
void RSCAN_1DWT_Client::moveZ(int pos, int vel)
{
    int command = RSCAN_1DWT_CommandCode::Move_Z;
    clearTxBuff();
    memcpy(tx + 0, &command, 4);
    memcpy(tx + 4, &pos, 4);
    memcpy(tx + 8, &vel, 4);
    TCPClient::copyToTx(tx, 0, kTxBufferSize); 
}
void RSCAN_1DWT_Client::stopZ()
{
    int command = RSCAN_1DWT_CommandCode::Stop_Z;
    clearTxBuff();
    memcpy(tx + 0, &command, 4);
    TCPClient::copyToTx(tx, 0, kTxBufferSize);
}
void RSCAN_1DWT_Client::resetZ()
{
    int command = RSCAN_1DWT_CommandCode::Reset_Z;
    clearTxBuff();
    memcpy(tx + 0, &command, 4);
    TCPClient::copyToTx(tx, 0, kTxBufferSize);
}

void RSCAN_1DWT_Client::clearTxBuff()
{
    memset(tx, 0, kTxBufferSize);
}