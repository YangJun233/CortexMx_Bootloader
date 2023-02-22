#include "protocol_adapter.h"

/**
  * @brief  firmware packet analysis,
  * @param  p_in: input data.
  * @param  size_in: input data size.
  * @param  p_out: output data,get valid data form input data.
  * @param  size_out: output data size.
  * @retval 0:ok, other: fail
  */
int protocol_fw_packet_analysis(PROTOCOL_ADAPTER_STR * adapter, uint8_t * p_in, uint32_t size_in, uint8_t * p_out, uint32_t size_out)
{
    return adapter->p_fw_packet_analysis(p_in, size_in, p_out, size_out);
}


/**
  * @brief  firmware packet analysis,
  * @param  p_in: input data.
  * @param  size_in: input data size.
  * @param  cmd: input cmd.
  * @param  p_out: output data,get valid data form input data.
  * @param  size_out: output data size.
  * @retval 0:ok, other: fail
  */
int protocol_cmd_packet_analysis(PROTOCOL_ADAPTER_STR * adapter, uint8_t * p_in, uint32_t size_in, 
                                 uint8_t * cmd, uint8_t * p_out, uint32_t size_out)
{
    return adapter->p_cmd_packet_analysis(p_in, size_in, cmd, p_out, size_out);
}

