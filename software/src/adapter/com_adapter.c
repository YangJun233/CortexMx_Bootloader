#include "com_adapter.h"

int read_data(COM_ADAPTER_STR * adapter, uint8_t * p, uint32_t size)
{
    return adapter->p_read_data(p, size);
}


int write_data(COM_ADAPTER_STR * adapter, uint8_t * p, uint32_t size)
{
    return adapter->p_write_data(p, size);
}