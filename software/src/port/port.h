#ifndef PORT_H
#define PORT_H

#include "com_adapter.h"
#include "storage_adapter.h"
#include "mcu_adapter.h"
#include "protocol_adapter.h"
#include "bootloader.h"

/**
 ©°©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©´
 ©¦                          ©¦
 ©¦        bootloader        ©¦
 ©¦                          ©¦
 ©À©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©È
 ©¦         bl data          ©¦
 ©À©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©È
 ©¦   exe area header(op)    ©¦
 ©À©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©È
 ©¦                          ©¦
 ©¦                          ©¦
 ©¦        exe area          ©¦
 ©¦                          ©¦
 ©¦                          ©¦
 ©À©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©È
 ©¦  backup area header(op)  ©¦
 ©À©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©È
 ©¦                          ©¦
 ©¦                          ©¦
 ©¦       backup area        ©¦
 ©¦                          ©¦
 ©¦                          ©¦
 ©À©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©È
 ©¦      customer data       ©¦
 ©¸©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¼
*/

/**
 * @brief enable backup area,if enable,the firmware first receives the backup area,than copy to the execution area.
 *        if disable,the firmware is written directly to the execution area.
 */
#define CONFIG_FW_BACKUP_AREA_ENABLE        1

/**
 * @brief enable fw header area,if enable,bootloader 
 */
#define CONFIG_FW_HEADER_ENABLE             0

/**
 * @brief enable fw verify.if enable,bootloader will verify execution area firmware.
 * @note  if fw verify is enabled,fw header(CONFIG_FW_HEADER_ENABLE) must be enabled.
 */
#define CONFIG_FW_VERIFY_ENABLE             0

#if (CONFIG_FW_VERIFY_ENABLE == 1)
#undef CONFIG_FW_HEADER_ENABLE
#define CONFIG_FW_HEADER_ENABLE             1
#endif



#define ADDR_BL_AREA_FW                 (0x8000000)
#define ADDR_BL_AREA_FW_SIZE            (12*1024)
#define ADDR_BL_AREA_DATA               (ADDR_BL_AREA_FW + ADDR_BL_AREA_FW_SIZE)
#define ADDR_BL_AREA_DATA_SIZE          (2*1024)

#if (CONFIG_FW_HEADER_ENABLE == 1)
#define ADDR_APP_AREA_HEADER            (ADDR_BL_AREA_DATA + ADDR_BL_AREA_DATA_SIZE)
#define ADDR_APP_AREA_HEADER_SIZE       (2*1024)
#endif
#define ADDR_APP_AREA_FW                (ADDR_BL_AREA_DATA + ADDR_BL_AREA_DATA_SIZE)
#define ADDR_APP_AREA_FW_SIZE           (20*1024)

#if (CONFIG_FW_BACKUP_AREA_ENABLE == 1)
#if (CONFIG_FW_HEADER_ENABLE == 1)
#define ADDR_BACKUP_AREA_HEADER         (ADDR_APP_AREA_FW + ADDR_APP_AREA_FW_SIZE)
#define ADDR_BACKUP_AREA_HEADER_SIZE    (2*1024)
#endif
#define ADDR_BACKUP_AREA_FW             (ADDR_APP_AREA_FW + ADDR_APP_AREA_FW_SIZE)
#define ADDR_BACKUP_AREA_FW_SIZE        (20*1024)
#endif

#define STORAGE_PAGE_SIZE               (2*1024)

extern com_adapter_str com_adapter;
extern storage_adapter_str storage_adapter;
extern mcu_adapter_str mcu_adapter;
extern protocol_adapter_str protocol_adapter;
extern bootloader_callback_str bl_callback;

#endif
