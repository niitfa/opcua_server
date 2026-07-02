#ifndef RWELL_CLIENT_H
#define RWELL_CLIENT_H

#include "tcp_client.h"
#include <vector>

// command list and sequence as it is in MCU program
enum RWELLCommandCode
{
    Empty = 0x00,
    Set_HV = 0x01,
    Set_Band = 0x02
};

// value list and sequence as it is in MCU program
enum RWELLValueCode
{
   	Message_Num,
    ADC_DR,
    HV_Out,
    Pressure,
    Temperature,
    Band,
	Size 
};

class RWELLClient : public TCPClient
{
    static const int kRxBufferSize = 64;
    static const int kTxBufferSize = 12;

    struct ValueAddress
    {
        uint32_t offset;
        uint32_t size;
    };

    std::vector<ValueAddress> map;
    uint8_t tx[kTxBufferSize];

public:
    RWELLClient(std::string ip, uint16_t port);
    ~RWELLClient();
    int getMessageNum();
    int getADCValue();
    int getHV();
    int getPressure();
    int getTemperature();
    int getBand();
    void setHV(uint16_t volt);
    void setBand(uint8_t band);
private:
    void setCommand(int code, int param_1, int param_2);
};

#endif