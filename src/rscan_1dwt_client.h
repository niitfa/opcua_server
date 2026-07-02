#ifndef RSCAN_1DWT_CLIENT_H
#define RSCAN_1DWT_CLIENT_H

#include "tcp_client.h"
#include <vector>

// command list and sequence as it is in MCU program
enum RSCAN_1DWT_CommandCode
{
    Empty = 0x00,
    Move_Z = 0x01,
    Stop_Z = 0x02,
    Reset_Z = 0x03
};

// value list and sequence as it is in MCU program
enum RSCAN_1DWT_ValueCode
{
   	Message_Num,
    Pos_Z,
	Size 
};

class RSCAN_1DWT_Client : public TCPClient
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
    RSCAN_1DWT_Client(std::string ip, uint16_t port);
    ~RSCAN_1DWT_Client();
    int getMessageNum();
    int getZ();
    void moveZ(int pos, int vel);
    void stopZ();
    void resetZ();

private:
    void clearTxBuff();
};

#endif