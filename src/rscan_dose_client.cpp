#include "rscan_dose_client.h"
#include <string.h>
#include <iostream>

RSCANRDoseClient::RSCANRDoseClient(std::string ip, uint16_t port) : TCPClient(ip, port, kRxBufferSize, kTxBufferSize)
{
    // value sizes and offsets as it is in MCU program
    map = std::vector<ValueAddress>(RSCANRDoseValueCode::Size);
    map[RSCANRDoseValueCode::Message_Num]   = {0, 4};
    map[RSCANRDoseValueCode::Dose_A]        = {4, 4};
    map[RSCANRDoseValueCode::Dose_B]        = {8, 4};
    map[RSCANRDoseValueCode::HV_A]          = {12, 4};
    map[RSCANRDoseValueCode::HV_B]          = {16, 4};
    map[RSCANRDoseValueCode::Temp_A]        = {20, 4};
    map[RSCANRDoseValueCode::Temp_B]        = {24, 4};
    map[RSCANRDoseValueCode::Range_A]       = {28, 1};
    map[RSCANRDoseValueCode::Range_B]       = {29, 1};
    map[RSCANRDoseValueCode::HV_Polarity_A] = {30, 1};
    map[RSCANRDoseValueCode::HV_Polarity_B] = {31, 1};
}

RSCANRDoseClient::~RSCANRDoseClient() 
{}

void RSCANRDoseClient::sendCommand(RSCANRDoseCommandCode command, int parameter)
{
    uint8_t tx[this->kTxBufferSize];
    memset(tx, 0, this->kTxBufferSize);
    memcpy(tx + 0, &command, 4); // copy command
    memcpy(tx + 4, &parameter, 4); // copy parameter
    TCPClient::copyToTx(tx, 0, this->kTxBufferSize);
}

int RSCANRDoseClient::readValue(RSCANRDoseValueCode valueCode)
{
    int res = 0;
    TCPClient::copyFromRx(&res, map[valueCode].offset, map[valueCode].size);
    return res;
}

