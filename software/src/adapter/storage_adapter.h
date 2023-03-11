#ifndef STORAGE_ADAPTER_H
#define STORAGE_ADAPTER_H

#include <stdint.h>

typedef struct
{
    int (*p_read_data)(uint32_t addr, uint8_t * p, uint32_t size);
    int (*p_write_data)(uint32_t addr, uint8_t * p, uint32_t size);
    int (*p_earse_data)(uint32_t addr, uint32_t size);
}storage_adapter_str;

int storage_read_data(storage_adapter_str * adapter, uint32_t addr, uint8_t * p, uint32_t size);
int storage_write_data(storage_adapter_str * adapter, uint32_t addr, uint8_t * p, uint32_t size);
int storage_earse_data(storage_adapter_str * adapter, uint32_t addr, uint32_t size);

#endif
