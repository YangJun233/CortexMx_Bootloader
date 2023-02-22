#ifndef STORAGE_ADAPTER_H
#define STORAGE_ADAPTER_H

#include <stdint.h>

typedef struct
{
    int (*p_read_8bit_data)(uint32_t addr, uint8_t * p, uint32_t size);
    int (*p_write_8bit_data)(uint32_t addr, uint8_t * p, uint32_t size);
    int (*p_earse_8bit_data)(uint32_t addr, uint8_t * p, uint32_t size);
}storage_adapter_str;

int storage_read_8bit_data(storage_adapter_str * adapter, uint32_t addr, uint8_t * p, uint32_t size);
int storage_write_8bit_data(storage_adapter_str * adapter, uint32_t addr, uint8_t * p, uint32_t size);
int storage_earse_8bit_data(storage_adapter_str * adapter, uint32_t addr, uint8_t * p, uint32_t size);

#endif
