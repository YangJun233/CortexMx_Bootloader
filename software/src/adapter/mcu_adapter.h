#ifndef MCU_ADAPTER_H
#define MCU_ADAPTER_H

#include <stdint.h>

typedef struct
{
    void (*p_reset)(void);
    int (*p_do_boot)(uint32_t addr);
}mcu_adapter_str;

void mcu_reset(mcu_adapter_str * p_adapter);
int mcu_do_boot(mcu_adapter_str * p_adapter, uint32_t addr);

#endif
