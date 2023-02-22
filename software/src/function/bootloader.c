#include "bootloader.h"
#include "port.h"
#include "storage_adapter.h"
#include "com_adapter.h"
#include "bl_config.h"

#define BL_ENTER_APP    0xA5A5A5A5
#define BL_UPDATING     0x69696969
#define BL_COPYING      0xC3C3C3C3


int bl_init(void)
{
    
}

uint32_t bl_get_state(void)
{
    uint32_t bl_state = 0;
    
    storage_read_8bit_data(&storage_adapter, 0x01, (uint8_t *)bl_state, 4);
    
    return bl_state;
}


int bl_update_firmware(void)
{
    
}