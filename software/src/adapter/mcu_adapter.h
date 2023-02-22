#ifndef MCU_ADAPTER_H
#define MCU_ADAPTER_H

#include <stdint.h>

typedef struct
{
    void (*p_reset)(void);
    void (*p_do_boot)(void * p_addr);
}mcu_adapter_str;

void mcu_reset(mcu_adapter_str * p_adapter);
void mcu_do_boot(mcu_adapter_str * p_adapter, void * addr);

#endif
