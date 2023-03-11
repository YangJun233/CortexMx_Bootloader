#include "com_adapter.h"

/**
  * @brief  communicaton read data.
  * @param  p: data point.
  * @param  size: read size.
  * @retval 0:ok, other: fail.
  */
int com_read_data(com_adapter_str * p_adapter, uint8_t * p, uint32_t * p_size)
{
    if(!p_adapter)
    {
        return -1;
    }
    
    return p_adapter->p_read_data(p, p_size);
}


/**
  * @brief  communicaton write data.
  * @param  p: data point.
  * @param  size: read size.
  * @retval 0:ok, other: fail.
  */
int com_write_data(com_adapter_str * p_adapter, uint8_t * p, uint32_t size)
{
    if(!p_adapter)
    {
        return -1;
    }
    
    return p_adapter->p_write_data(p, size);
}

