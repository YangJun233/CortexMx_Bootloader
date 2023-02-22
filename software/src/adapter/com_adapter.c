#include "com_adapter.h"

/**
  * @brief  communicaton read data.
  * @param  p: data point.
  * @param  size: read size.
  * @retval 0:ok, other: fail.
  */
int com_read_data(com_adapter_str * adapter, uint8_t * p, uint32_t size)
{
    return adapter->p_read_data(p, size);
}


/**
  * @brief  communicaton write data.
  * @param  p: data point.
  * @param  size: read size.
  * @retval 0:ok, other: fail.
  */
int com_write_data(com_adapter_str * adapter, uint8_t * p, uint32_t size)
{
    return adapter->p_write_data(p, size);
}