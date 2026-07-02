#ifndef RSCAN_DOSE_CLIENT_H
#define RSCAN_DOSE_CLIENT_H

#include "tcp_client.h"
#include <vector>

// command list and sequence as it is in MCU program
enum class RSCANRDoseCommandCode
{
 	Empty = 0x00000000l,
	Enable_HV_A,
	Enable_HV_B,
	Disable_HV_A,
	Disable_HV_B,
	Set_Voltage_A,
	Set_Voltage_B,
	Set_HV_Polarity_A,
	Set_HV_Polarity_B,
	Set_Broad_Range_A,
	Set_Broad_Range_B,
	Set_Narrow_Range_A,
	Set_Narrow_Range_B   
};

// value list and sequence as it is in MCU program
enum RSCANRDoseValueCode
{
   	Message_Num,
	Dose_A,
	Dose_B,
	HV_A,
	HV_B,
	Temp_A,
	Temp_B,
	Range_A,
	Range_B,
	HV_Polarity_A,
	HV_Polarity_B,
	Size 
};

class RSCANRDoseClient : public TCPClient
{
    static const int kRxBufferSize = 64;
    static const int kTxBufferSize = 64;

    struct ValueAddress
    {
        uint32_t offset;
        uint32_t size;
    };

    std::vector<ValueAddress> map;
public:
    RSCANRDoseClient(std::string ip, uint16_t port);
    ~RSCANRDoseClient();
    void sendCommand(RSCANRDoseCommandCode command, int parameter = 0);
    int readValue(RSCANRDoseValueCode valueCode);
};

#endif