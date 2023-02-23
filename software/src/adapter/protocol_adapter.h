#ifndef PROTOCOL_ADAPTER_H
#define PROTOCOL_ADAPTER_H

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    uint8_t     type;       //packet type
    uint8_t *   p_buffer;   //packet point
    uint32_t    size;       //packet size
}packet_attribute_str;

enum
{
    FW_PACKET_TYPE = 0x01,
    LAST_FW_PACKET_TYPE = 0x02,
}PACKET_TYPE_ENUM;

typedef struct
{
    uint32_t fw_packet_max_size;
    uint32_t ack_packet_max_size;
    
    /**
      * @brief  firmware packet analysis,
      * @param  p_in: input data.
      * @param  size_in: input data size.
      * @param  type: data type, 0x01: 0-(last-1) packet falg, 0x02: last packet flag.
      * @param  p_out: output data,get firmware data form input data.
      * @param  p_size_out: output data size.
      * @retval 0:ok, other: fail.
      */
    int (*p_packet_analysis)(uint8_t * p_in, uint32_t size_in, uint8_t * type, uint8_t * p_out, uint32_t * p_size_out);
    
    /**
      * @brief  build firmware ack package.
      * @param  ack: true:rce and process fw packet ok, false:rce and process fw packet fail.
      * @param  p: ack packet data.
      * @param  size: ack packet size.
      * @retval
      */
    void (*p_build_ack_packet)(bool ack, uint8_t * p, uint32_t * size);
}protocol_adapter_str;

int protocol_packet_analysis(protocol_adapter_str * adapter, uint8_t * p_in, uint32_t size_in, packet_attribute_str * p_packet_att);
void protocol_build_ack_packet(protocol_adapter_str * p_adapter, bool ack, uint8_t * p, uint32_t * size);

#endif

