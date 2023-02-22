#ifndef BOOTLOADER_H
#define BOOTLOADER_H

#include <stdint.h>

typedef struct
{
    uint8_t state;
}bootloader_manager_str;


typedef struct
{
    uint8_t state;
}update_manager_str;


typedef struct
{
    void (*p_copy_fw_ok_cb)(void);
    void (*p_copy_fw_fail_cb)(void);
    
    void (*p_update_fw_ok_cb)(void);
    void (*p_update_fw_fail_cb)(void);
    
    
}bootloader_callback_str;

int bl_copy_firmware(void);

#endif
