#ifndef CONFIG_H
#define CONFIG_H

/**
 * @brief enable fw header area,if enable,bootloader 
 */
#define CONFIG_FW_HEADER_ENABLE             1

/**
 * @brief enable backup area,if enable,the firmware first receives the backup area,than copy to the execution area.
 *        if disable,the firmware is written directly to the execution area.
 */
#define CONFIG_FW_BACKUP_AREA_ENABLE        1

/**
 * @brief enable fw verify.if enable,bootloader will verify execution area firmware.
 * @note  if fw verify is enabled,fw header(CONFIG_FW_HEADER_ENABLE) must be enabled.
 */
#define CONFIG_FW_VERIFY_ENABLE             0

#if (CONFIG_FW_VERIFY_ENABLE == 1)
#undef CONFIG_FW_HEADER_ENABLE
#define CONFIG_FW_HEADER_ENABLE             1
#endif



#endif
