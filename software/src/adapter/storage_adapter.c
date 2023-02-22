#include "storage_adapter.h"

/**
  * @brief  read data form storage.
  * @param  addr: address.
  * @param  p: read data point.
  * @param  size: read size.
  * @retval 0:ok, other: fail.
  */
int storage_read_8bit_data(STORAGE_ADAPTER_STR * adapter, uint32_t addr, uint8_t * p, uint32_t size)
{
    return adapter->p_read_8bit_data(addr, p, size);
}

/**
  * @brief  write data form storage.
  * @param  addr: address.
  * @param  p: data point.
  * @param  size: read size.
  * @retval 0:ok, other: fail.
  */
int storage_write_8bit_data(STORAGE_ADAPTER_STR * adapter, uint32_t addr, uint8_t * p, uint32_t size)
{
    return adapter->p_write_8bit_data(addr, p, size);
}

/**
  * @brief  earse data form storage.
  * @param  addr: address.
  * @param  p: data point.
  * @param  size: read size.
  * @retval 0:ok, other: fail.
  */
int storage_earse_8bit_data(STORAGE_ADAPTER_STR * adapter, uint32_t addr, uint8_t * p, uint32_t size)
{
    return adapter->p_earse_8bit_data(addr, p, size);
}