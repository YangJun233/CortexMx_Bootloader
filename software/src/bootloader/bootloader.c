#include "bootloader.h"
#include "port.h"
#include "storage_adapter.h"
#include "com_adapter.h"
#include "mcu_adapter.h"

#include <stdio.h>

#define BL_ENTER_APP    0xA5A5A5A5
#define BL_UPDATING     0x69696969
#define BL_COPYING      0xC3C3C3C3

#define assert_error(a)             \
{                                   \
    if (!(a))                       \
    {                               \
        do {} while (1);            \
    }                               \
}

bootloader_manager_str bl_manager;

int bl_get_state(uint8_t * p_state)
{
    int ret =  storage_read_data(&storage_adapter, ADDR_BL_AREA_DATA, p_state, 4);
    
    return ret;
}

int bl_set_state(uint32_t state)
{
    uint32_t bl_state = state;
    
    int ret = -1;
    
    ret = storage_earse_data(&storage_adapter, ADDR_BL_AREA_DATA, 4);
    if(ret != 0)
    {
        return -1;
    }

    ret = storage_write_data(&storage_adapter, ADDR_BL_AREA_DATA, (uint8_t *)bl_state, 4);
    if(ret != 0)
    {
        return -1;
    }
    
    return 0;
}

static void enter_mcu_reset(void)
{
    if(bl_callback.p_mcu_reset_before_cb != NULL)
    {
        bl_callback.p_mcu_reset_before_cb();
    }

    mcu_reset(&mcu_adapter);
}

static void bl_enter_app_flow(void)
{
#if (CONFIG_FW_VERIFY_ENABLE == 1)
    bl_manager.enter_app_flow_node = ENTER_APP__VERIFY;
#else
    bl_manager.enter_app_flow_node = ENTER_APP__JUMP_APP;
#endif
    
    while(1)
    {
        switch(bl_manager.enter_app_flow_node)
        {
#if (CONFIG_FW_VERIFY_ENABLE == 1)
            case ENTER_APP__VERIFY:
                if(fw_verify() == 0)
                {
                    bl_manager.enter_app_flow_node = ENTER_APP__VERIFY_OK;
                }
                else
                {
                    bl_manager.enter_app_flow_node = ENTER_APP__VERIFY_FAIL;
                }
                break;
            
            case ENTER_APP__VERIFY_OK:
                if(bl_callback.p_enter_app_verify_ok_cb != NULL)
                {
                    bl_callback.p_enter_app_verify_ok_cb();
                }

                bl_manager.enter_app_flow_node = ENTER_APP__JUMP_APP;
                break;
            
            case ENTER_APP__VERIFY_FAIL:
                if(bl_callback.p_enter_app_verify_fail_cb != NULL)
                {
                    bl_callback.p_enter_app_verify_fail_cb();
                }

                bl_manager.enter_app_flow_node = ENTER_APP__RESET;
                break;
#endif
            case ENTER_APP__JUMP_APP:
                if(bl_callback.p_enter_app_jump_before_cb != NULL)
                {
                    bl_callback.p_enter_app_jump_before_cb();
                }

                if(mcu_do_boot(&mcu_adapter, ADDR_APP_AREA_FW) != 0)
                {
                    bl_set_state(BL_UPDATING);
                    bl_manager.enter_app_flow_node = ENTER_APP__RESET;
                }
                break;
            
            case ENTER_APP__RESET:
                enter_mcu_reset();
                break;
            default:
                break;
        }
    }
}

static void bl_copy_flow(void)
{
    int ret = -1;
    
    uint8_t buffer[STORAGE_PAGE_SIZE];
    
#if (FW_HEADER_ENABLE == 1)
    uint32_t addr_copy_form = ADDR_BACKUP_AREA_HEADER;
    uint32_t addr_copy_to = ADDR_EXE_AREA_HEADER;
    
    uint32_t copy_size = ADDR_EXE_AREA_HEADER_SIZE;
    
    bl_manager.copy_flow_node = COPY_FLOW__COPY_HEADER;
#else
    uint32_t addr_copy_form = ADDR_BACKUP_AREA_FW;
    uint32_t addr_copy_to = ADDR_APP_AREA_FW;

    uint32_t copy_size = ADDR_APP_AREA_FW_SIZE;
    
    bl_manager.copy_flow_node = COPY_FLOW__COPY_FW;
#endif
    uint32_t addr_copy_offset = 0;
    
//    uint32_t page_num = (copy_size + STORAGE_PAGE_SIZE - 1) / STORAGE_PAGE_SIZE;

    while(1)
    {
        switch(bl_manager.copy_flow_node)
        {
#if (CONFIG_FW_HEADER_ENABLE == 1)
            case COPY_FLOW__COPY_HEADER:
                storage_earse_data(&storage_adapter, addr_copy_to, ADDR_BACKUP_AREA_HEADER_SIZE);

                while(addr_copy_offset < copy_size)
                {
                    ret = storage_read_data(&storage_adapter, addr_copy_form+addr_copy_offset, buffer, STORAGE_PAGE_SIZE);
                    if(ret != 0)
                    {
                        bl_manager.copy_flow_node = COPY_FLOW__COPY_HEADER_FAIL;
                    }

                    ret = storage_write_data(&storage_adapter, addr_copy_to+addr_copy_offset, buffer, STORAGE_PAGE_SIZE);
                    if(ret != 0)
                    {
                        bl_manager.copy_flow_node = COPY_FLOW__COPY_HEADER_FAIL;
                    }

                    addr_copy_offset += STORAGE_PAGE_SIZE;
                }

                bl_manager.copy_flow_node = COPY_FLOW__COPY_HEADER_OK;
                break;

            case COPY_FLOW__COPY_HEADER_OK:
                if(bl_callback.p_copy_header_ok_cb != NULL)
                {
                    bl_callback.p_copy_header_ok_cb();
                }

                addr_copy_form = ADDR_BACKUP_AREA_FW;
                addr_copy_to = ADDR_APP_AREA_FW;
                copy_size = ADDR_APP_AREA_FW_SIZE;
                addr_copy_offset = 0;
                
                bl_manager.copy_flow_node = COPY_FLOW__COPY_FW;
                break;

            case COPY_FLOW__COPY_HEADER_FAIL:
                if(bl_callback.p_copy_header_fail_cb != NULL)
                {
                    bl_callback.p_copy_header_fail_cb();
                }
                break;
#endif
            case COPY_FLOW__COPY_FW:
                ret = storage_earse_data(&storage_adapter, addr_copy_to, copy_size);
                if(ret != 0)
                {
                    bl_manager.copy_flow_node = COPY_FLOW__COPY_FW_FAIL;
                }

                while(addr_copy_offset < copy_size)
                {
                    ret = storage_read_data(&storage_adapter, addr_copy_form+addr_copy_offset, buffer, STORAGE_PAGE_SIZE);
                    if(ret != 0)
                    {
                        bl_manager.copy_flow_node = COPY_FLOW__COPY_FW_FAIL;
                    }

                    ret = storage_write_data(&storage_adapter, addr_copy_to+addr_copy_offset, buffer, STORAGE_PAGE_SIZE);
                    if(ret != 0)
                    {
                        bl_manager.copy_flow_node = COPY_FLOW__COPY_FW_FAIL;
                    }

                    addr_copy_offset += STORAGE_PAGE_SIZE;
                }

                bl_manager.copy_flow_node = COPY_FLOW__COPY_FW_OK;
                break;

            case COPY_FLOW__COPY_FW_OK:
                if(bl_callback.p_copy_fw_ok_cb != NULL)
                {
                    bl_callback.p_copy_fw_ok_cb(addr_copy_offset);
                }

#if (CONFIG_FW_VERIFY_ENABLE == 1)
                bl_manager.copy_flow_node = COPY_FLOW__VERIFY;
#else
                bl_set_state(BL_ENTER_APP);
                bl_manager.copy_flow_node = COPY_FLOW__RESET;
#endif
                
                break;
            case COPY_FLOW__COPY_FW_FAIL:
                if(bl_callback.p_copy_fw_fail_cb != NULL)
                {
                    bl_callback.p_copy_fw_fail_cb(addr_copy_offset);
                }

                bl_manager.copy_flow_node = COPY_FLOW__RESET;
                break;

#if (CONFIG_FW_VERIFY_ENABLE == 1)
            case COPY_FLOW__VERIFY:
                break;

            case COPY_FLOW__VERIFY_OK:
                if(bl_callback.p_copy_verify_ok_cb != NULL)
                {
                    bl_callback.p_copy_verify_ok_cb();
                }

                bl_set_state(BL_ENTER_APP);
                bl_manager.copy_flow_node = COPY_FLOW__RESET;
                break;

            case COPY_FLOW__VERIFY_FAIL:
                if(bl_callback.p_copy_verify_fail_cb != NULL)
                {
                    bl_callback.p_copy_verify_fail_cb();
                }

                bl_manager.copy_flow_node = COPY_FLOW__RESET;
                break;
#endif

            case COPY_FLOW__RESET:
                enter_mcu_reset();
                break;

            default:
                break;
        }
    }
}

static void bl_update_flow(void)
{
    uint8_t buffer[STORAGE_PAGE_SIZE];
    uint32_t rce_size;
    
    uint8_t fw_buffer[STORAGE_PAGE_SIZE];
    packet_attribute_str packet_att = 
    {
        .p_buffer = fw_buffer,
    };
    
    int ret = -1;
    
#if (CONFIG_FW_BACKUP_AREA_ENABLE == 1)
    #if (CONFIG_FW_HEADER_ENABLE == 1)
        uint32_t addr_write_to = ADDR_BACKUP_AREA_HEADER;
        uint32_t max_write_size = ADDR_BACKUP_AREA_HEADER_SIZE;
    
        bl_manager.update_flow_node = UPDATE_FLOW__READY_TO_RCE_HEADER;
    #else
        uint32_t addr_write_to = ADDR_BACKUP_AREA_FW;
        uint32_t max_write_size = ADDR_BACKUP_AREA_FW_SIZE;
    
        bl_manager.update_flow_node = UPDATE_FLOW__READY_TO_RCE_FW;
    #endif
#else
    #if (CONFIG_FW_HEADER_ENABLE == 1)
        uint32_t addr_write_to = ADDR_EXE_AREA_HEADER;
        uint32_t max_write_size = ADDR_EXE_AREA_HEADER_SIZE;
    
        bl_manager.update_flow_node = UPDATE_FLOW__READY_TO_RCE_HEADER;
    #else
        uint32_t addr_write_to = ADDR_EXE_AREA_FW;
        uint32_t max_write_size = ADDR_EXE_AREA_FW_SIZE;
        
        bl_manager.update_flow_node = UPDATE_FLOW__READY_TO_RCE_FW;
    #endif
#endif
    uint32_t addr_write_offset = 0;
    
    while(1)
    {
        switch(bl_manager.update_flow_node)
        {
#if (CONFIG_FW_HEADER_ENABLE == 1)
            case UPDATE_FLOW__READY_TO_RCE_HEADER:
                ret = storage_earse_data(&storage_adapter, addr_write_to, max_write_size);
                if(ret != 0)
                {
                    bl_manager.copy_flow_node = UPDATE_FLOW__RCE_HEADER_FAIL;
                }

                bl_manager.copy_flow_node = UPDATE_FLOW__REC_HEADER;
                break;

            case UPDATE_FLOW__REC_HEADER:
                break;

            case UPDATE_FLOW__RCE_HEADER_OK:
                if(bl_callback.p_update_header_ok_cb != NULL)
                {
                    bl_callback.p_update_header_ok_cb();
                }

                addr_write_to = ADDR_EXE_AREA_FW;
                max_write_size = ADDR_EXE_AREA_FW_SIZE;
                addr_write_offset = 0;
                
                bl_manager.update_flow_node = UPDATE_FLOW__READY_TO_RCE_FW;
                break;

            case UPDATE_FLOW__RCE_HEADER_FAIL:
                if(bl_callback.p_update_header_fail_cb != NULL)
                {
                    bl_callback.p_update_header_fail_cb();
                }
                break;
#endif
            case UPDATE_FLOW__READY_TO_RCE_FW:
                ret = storage_earse_data(&storage_adapter, addr_write_to, max_write_size);
                if(ret != 0)
                {
                    bl_manager.copy_flow_node = UPDATE_FLOW__RCE_FW_FAIL;
                }

                bl_manager.copy_flow_node = UPDATE_FLOW__REC_FW;
                break;

            case UPDATE_FLOW__REC_FW:
                while(com_read_data(&com_adapter, buffer, &rce_size) == 0)
                {
                    ret = protocol_packet_analysis(&protocol_adapter, buffer, rce_size, &packet_att);
                    if(ret != 0)
                    {
                        //todo: rce failed callback
                        if(bl_callback.p_rce_fw_packet_fail_cb != NULL)
                        {
                            bl_callback.p_rce_fw_packet_fail_cb(addr_write_offset);

                            continue;
                        }
                    }

                    //write to flash
                    if(packet_att.size > 0)
                    {
                        if(addr_write_offset + packet_att.size > max_write_size)
                        {
                            bl_manager.update_flow_node = UPDATE_FLOW__RCE_FW_FAIL;

                            break;
                        }

                        ret = storage_write_data(&storage_adapter, addr_write_to+addr_write_offset, packet_att.p_buffer, packet_att.size);
                        if(ret != 0)
                        {
                            //todo: rce failed callback
                            if(bl_callback.p_rce_fw_packet_fail_cb != NULL)
                            {
                                bl_callback.p_rce_fw_packet_fail_cb(addr_write_offset);

                                continue;
                            }
                        }

                        //todo: rce ok callback
                        if(bl_callback.p_rce_fw_packet_ok_cb != NULL)
                        {
                            bl_callback.p_rce_fw_packet_ok_cb(addr_write_offset);
                        }

                        addr_write_offset += packet_att.size;
                    }

                    if(packet_att.type == LAST_FW_PACKET_TYPE)
                    {
                        bl_manager.update_flow_node = UPDATE_FLOW__RCE_FW_OK;

                        break;
                    }
                }
                break;

            case UPDATE_FLOW__RCE_FW_OK:
                if(bl_callback.p_update_fw_ok_cb != NULL)
                {
                    bl_callback.p_update_fw_ok_cb(addr_write_offset);
                }

#if (CONFIG_FW_VERIFY_ENABLE == 1)
                bl_manager.copy_flow_node = UPDATE_FLOW__VERIFY;
#else
                bl_set_state(BL_ENTER_APP);
                bl_manager.copy_flow_node = UPDATE_FLOW__RESET;
#endif
                break;

            case UPDATE_FLOW__RCE_FW_FAIL:
                if(bl_callback.p_update_fw_fail_cb != NULL)
                {
                    bl_callback.p_update_fw_fail_cb(addr_write_offset);
                }
                break;

#if (CONFIG_FW_VERIFY_ENABLE == 1)
            case UPDATE_FLOW__VERIFY:
                break;

            case UPDATE_FLOW__VERIFY_OK:
                if(bl_callback.p_update_verify_ok_cb != NULL)
                {
                    bl_callback.p_update_verify_ok_cb();
                }

                bl_set_state(BL_ENTER_APP);
                bl_manager.copy_flow_node = UPDATE_FLOW__RESET;
                break;

            case UPDATE_FLOW__VERIFY_FAIL:
                if(bl_callback.p_update_verify_fail_cb != NULL)
                {
                    bl_callback.p_update_verify_fail_cb();
                }

                bl_manager.copy_flow_node = UPDATE_FLOW__RESET;
                break;
#endif
            case UPDATE_FLOW__RESET:
                enter_mcu_reset();
                break;
            default:
                break;
        }
    }
}


void bl_main(void)
{
    uint32_t bl_state = (uint32_t)-1;
    
    bl_get_state((uint8_t *)bl_state);
    
    switch(bl_state)
    {
        case BL_ENTER_APP:
            assert_error(mcu_adapter.p_do_boot);
            assert_error(mcu_adapter.p_reset);
        
            bl_enter_app_flow();
            break;

        case BL_COPYING:
            assert_error(storage_adapter.p_earse_data);
            assert_error(storage_adapter.p_read_data);
            assert_error(storage_adapter.p_write_data);
            assert_error(storage_adapter.p_write_data);
            assert_error(mcu_adapter.p_reset);
        
            bl_copy_flow();
            break;

        case BL_UPDATING:
            assert_error(protocol_adapter.p_packet_analysis);
            assert_error(com_adapter.p_read_data);
            assert_error(com_adapter.p_write_data);
            assert_error(storage_adapter.p_earse_data);
            assert_error(storage_adapter.p_read_data);
            assert_error(storage_adapter.p_write_data);
            assert_error(mcu_adapter.p_reset);
        
            bl_update_flow();
            break;

        default:
            break;
    }
}


