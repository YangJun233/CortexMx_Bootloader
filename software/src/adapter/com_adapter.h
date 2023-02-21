#ifndef COM_ADAPTER_H
#define COM_ADAPTER_H

#include <stdint.h>

typedef struct
{
    int (*p_read_data)(uint8_t * p, uint32_t size);
    int (*p_write_data)(uint8_t * p, uint32_t size);
}COM_ADAPTER_STR;

#endif
