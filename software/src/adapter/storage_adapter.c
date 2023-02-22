#include "storage_adapter.h"

/**
  * @brief  read data form storage.
  * @param  addr: address.
  * @param  p: read data point.
  * @param  size: read size.
  * @retval 0:ok, other: fail.
  */
int storage_read_8bit_data(storage_adapter_str * p_adapter, uint32_t addr, uint8_t * p, uint32_t size)
{
    if(!p_adapter)
    {
        return -1;
    }
    
    return p_adapter->p_read_8bit_data(addr, p, size);
}

/**
  * @brief  write data form storage.
  * @param  addr: address.
  * @param  p: data point.
  * @param  size: read size.
  * @retval 0:ok, other: fail.
  */
int storage_write_8bit_data(storage_adapter_str * p_adapter, uint32_t addr, uint8_t * p, uint32_t size)
{
    if(!p_adapter)
    {
        return -1;
    }
    
    return p_adapter->p_write_8bit_data(addr, p, size);
}

/**
  * @brief  earse data form storage.
  * @param  addr: address.
  * @param  p: data point.
  * @param  size: read size.
  * @retval 0:ok, other: fail.
  */
int storage_earse_data(storage_adapter_str * p_adapter, uint32_t addr, uint32_t size)
{
    if(!p_adapter)
    {
        return -1;
    }
    
    return p_adapter->p_earse_data(addr, size);
}