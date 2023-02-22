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

#endif
