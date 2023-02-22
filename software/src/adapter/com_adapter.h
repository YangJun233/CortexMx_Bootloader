#ifndef COM_ADAPTER_H
#define COM_ADAPTER_H

#include <stdint.h>

typedef struct
{
    int (*p_read_data)(uint8_t * p, uint32_t * p_size);
    int (*p_write_data)(uint8_t * p, uint32_t size);
}com_adapter_str;

int com_read_data(com_adapter_str * adapter, uint8_t * p, uint32_t * p_size);
int com_write_data(com_adapter_str * adapter, uint8_t * p, uint32_t size);

#endif
