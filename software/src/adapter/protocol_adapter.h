#ifndef PROTOCOL_ADAPTER_H
#define PROTOCOL_ADAPTER_H

#include <stdint.h>

typedef struct
{
    int (*p_fw_packet_analysis)(uint8_t * p_in, uint32_t size_in, uint8_t * p_out, uint32_t size_out);
    int (*p_cmd_packet_analysis)(uint8_t * p_in, uint32_t size_in, uint8_t * cmd, uint8_t * p_out, uint32_t size_out);
}protocol_adapter_str;

int protocol_fw_packet_analysis(protocol_adapter_str * adapter, uint8_t * p_in, uint32_t size_in, uint8_t * p_out, uint32_t size_out);
int protocol_cmd_packet_analysis(protocol_adapter_str * adapter, uint8_t * p_in, uint32_t size_in, 
                                 uint8_t * cmd, uint8_t * p_out, uint32_t size_out);
#endif
