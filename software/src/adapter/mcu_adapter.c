#include "mcu_adapter.h"

/**
  * @brief  mcu soft reset.
  * @param  p_adapter: mcu adapter.
  * @retval
  */
void mcu_reset(mcu_adapter_str * p_adapter)
{
    if(!p_adapter)
    {
        return;
    }
    
    p_adapter->p_reset();
}


/**
  * @brief  mcu soft reset.
  * @param  p_adapter: mcu adapter.
  * @param  addr: boot address.
  * @retval
  */
void mcu_do_boot(mcu_adapter_str * p_adapter, void * addr)
{
    if(!p_adapter)
    {
        return;
    }
    
    p_adapter->p_do_boot((void *)addr);
}