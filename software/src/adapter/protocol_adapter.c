#include "protocol_adapter.h"
#include <stdio.h>

/**
  * @brief  firmware packet analysis,
  * @param  p_adapter: protocol adapter.
  * @param  p_in: input data.
  * @param  size_in: input data size.
  * @param  p_out: output data,get valid data form input data.
  * @param  size_out: output data size.
  * @retval 0:ok, other: fail
  */
int protocol_packet_analysis(protocol_adapter_str * p_adapter, uint8_t * p_in, uint32_t size_in, packet_attribute_str * p_packet_att)
{
    if(!p_adapter)
    {
        return -1;
    }
    
    return p_adapter->p_packet_analysis(p_in, size_in, &p_packet_att->type, p_packet_att->p_buffer, &p_packet_att->size);
}


/**
  * @brief  build firmware ack package.
  * @param  ack: true:rce and process fw packet ok, false:rce and process fw packet fail.
  * @param  p: ack packet data.
  * @param  size: ack packet size.
  * @retval
  */
void protocol_build_ack_packet(protocol_adapter_str * p_adapter, bool ack, uint8_t * p, uint32_t * size)
{
    if(!p_adapter)
    {
        return;
    }
    if(p_adapter->p_build_ack_packet != NULL)
    {
        p_adapter->p_build_ack_packet(ack, p, size);
    }
    else
    {
        *size = 0;
    }
}

