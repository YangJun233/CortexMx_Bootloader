#ifndef BOOTLOADER_H
#define BOOTLOADER_H

#include <stdint.h>

enum
{
#if (CONFIG_FW_VERIFY_ENABLE == 1)
    ENTER_APP__VERIFY,
    ENTER_APP__VERIFY_OK,
    ENTER_APP__VERIFY_FAIL,
#endif
    ENTER_APP__JUMP_APP,
    ENTER_APP__RESET,
    
    
#if (CONFIG_FW_HEADER_ENABLE == 1)
    COPY_FLOW__COPY_HEADER,
    COPY_FLOW__COPY_HEADER_OK,
    COPY_FLOW__COPY_HEADER_FAIL,
#endif
    COPY_FLOW__COPY_FW,
    COPY_FLOW__COPY_FW_OK,
    COPY_FLOW__COPY_FW_FAIL,
#if (CONFIG_FW_VERIFY_ENABLE == 1)
    COPY_FLOW__VERIFY,
    COPY_FLOW__VERIFY_OK,
    COPY_FLOW__VERIFY_FAIL,
#endif
    COPY_FLOW__RESET,
    

#if (CONFIG_FW_HEADER_ENABLE == 1)
    UPDATE_FLOW__READY_TO_RCE_HEADER,
    UPDATE_FLOW__REC_HEADER,
    UPDATE_FLOW__RCE_HEADER_OK,
    UPDATE_FLOW__RCE_HEADER_FAIL,
#endif
    UPDATE_FLOW__READY_TO_RCE_FW,
    UPDATE_FLOW__REC_FW,
    UPDATE_FLOW__RCE_FW_OK,
    UPDATE_FLOW__RCE_FW_FAIL,
#if (CONFIG_FW_VERIFY_ENABLE == 1)
    UPDATE_FLOW__VERIFY,
    UPDATE_FLOW__VERIFY_OK,
    UPDATE_FLOW__VERIFY_FAIL,
#endif
    UPDATE_FLOW__RESET,
};

typedef struct
{
    uint8_t enter_app_flow_node;
    uint8_t copy_flow_node;
    uint8_t update_flow_node;
}bootloader_manager_str;


typedef struct
{
#if (CONFIG_FW_VERIFY_ENABLE == 1)
    void (*p_enter_app_verify_ok_cb)(void);
    void (*p_enter_app_verify_fail_cb)(void);
#endif
    void (*p_enter_app_jump_before_cb)(void);


#if (CONFIG_FW_HEADER_ENABLE == 1)
    void (*p_copy_header_ok_cb)(uint32_t addr_offset);
    void (*p_copy_header_fail_cb)(uint32_t addr_offset);
#endif
    void (*p_copy_fw_ok_cb)(uint32_t addr_offset);
    void (*p_copy_fw_fail_cb)(uint32_t addr_offset);
#if (CONFIG_FW_VERIFY_ENABLE == 1)
    void (*p_copy_verify_ok_cb)(void);
    void (*p_copy_verify_fail_cb)(void);
#endif
    

#if (CONFIG_FW_HEADER_ENABLE == 1)
    void (*p_rce_header_packet_ok_cb)(uint32_t addr_offset);
    void (*p_rce_header_packet_fail_cb)(uint32_t addr_offset);

    void (*p_update_header_ok_cb)(uint32_t addr_offset);
    void (*p_update_header_fail_cb)(uint32_t addr_offset);
#endif

    void (*p_rce_fw_packet_ok_cb)(uint32_t addr_offset);
    void (*p_rce_fw_packet_fail_cb)(uint32_t addr_offset);

    void (*p_update_fw_ok_cb)(uint32_t addr_offset);
    void (*p_update_fw_fail_cb)(uint32_t addr_offset);

#if (CONFIG_FW_VERIFY_ENABLE == 1)
    void (*p_update_verify_ok_cb)(void);
    void (*p_update_verify_fail_cb)(void);
#endif

    void (*p_mcu_reset_before_cb)(void);
}bootloader_callback_str;

void bl_main(void);

#endif
