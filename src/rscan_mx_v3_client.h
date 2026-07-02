#ifndef RSCAN_MX_V3_CLIENT_H
#define RSCAN_MX_V3_CLIENT_H

#include "tcp_client.h"
#include <vector>

// command list and sequence as it is in MCU program
enum RSCANMXV3CommandCode
{
    Empty = 0x00000000l
};

// value list and sequence as it is in MCU program
enum RSCANMXV3ValueCode
{
   	Message_Num,
    HV,
    Data,
	Size 
};

class RSCANMXV3Client : public TCPClient
{
    static const int kRxBufferSize = 3426;
    static const int kTxBufferSize = 8;

    struct ValueAddress
    {
        uint32_t offset;
        uint32_t size;
    };

    std::vector<ValueAddress> map;
    uint8_t tx[kTxBufferSize];

public:
    RSCANMXV3Client(std::string ip, uint16_t port);
    ~RSCANMXV3Client();
    int getMessageNum();
    uint16_t getHV();
    int getCellSignal(int cellNo);
};

#endif