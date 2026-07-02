#ifndef RSTEP_CLIENT_H
#define RSTEP_CLIENT_H

#include "tcp_client.h"
#include <vector>

// command list and sequence as it is in MCU program
enum RSTEPCommandCode
{
    Empty = 0x00,
    Move_Long = 0x01,
    Move_Ang = 0x02,
    Stop_Long = 0x03,
    Stop_Ang = 0x04,
    Reset_Long = 0x05,
    Reset_Ang = 0x06
};


// value list and sequence as it is in MCU program
enum RSTEPValueCode
{
   	Message_Num,
    Pos_Long,
    Pos_Ang,
	Size 
};

class RSTEPClient : public TCPClient
{
    static const int kRxBufferSize = 64;
    static const int kTxBufferSize = 64;

    struct ValueAddress
    {
        uint32_t offset;
        uint32_t size;
    };

    std::vector<ValueAddress> map;
    uint8_t tx[kTxBufferSize];

public:
    RSTEPClient(std::string ip, uint16_t port);
    ~RSTEPClient();
    int getMessageNum();
    int getLongPosition();
    int getAngPosition();
    void moveLong(int pos, int velosity);
    void moveAng(int pos, int velosity);
    void stopLong();
    void stopAng();
    void resetLong(int pos, int velosity);
    void resetAng(int pos, int velosity);   
private:
    void setCommand(int code, int param_1, int param_2);
};

#endif