#include "bootloader.h"
#include "port.h"
#include "storage_adapter.h"
#include "com_adapter.h"
#include "mcu_adapter.h"
#include "bl_config.h"

#include <stdio.h>

#define BL_ENTER_APP    0xA5A5A5A5
#define BL_UPDATING     0x69696969
#define BL_COPYING      0xC3C3C3C3

int bl_get_state(uint8_t * p_state)
{
    int ret =  storage_read_8bit_data(&storage_adapter, ADDR_BL_AREA_DATA, p_state, 4);
    
    return ret;
}

int bl_set_state(uint32_t state)
{
    uint32_t bl_state = state;
    
    int ret = storage_write_8bit_data(&storage_adapter, ADDR_BL_AREA_DATA, (uint8_t *)bl_state, 4);
    
    return ret;
}



int bl_init_check(void)
{
    uint32_t bl_state = -1;
    int ret = -1;
    
    ret = bl_get_state((uint8_t *)bl_state);
    if(ret < 0)
    {
        return ret;
    }
    
    if(bl_state == BL_ENTER_APP)
    {
#if (CONFIG_FW_VERIFY_ENABLE == 1)
        //todo:verify fw
        ret = fw_verify();
        if(ret < 0)   //verify fail
        {
            return ret;
        }
#endif
        //jump to exe area
        mcu_do_boot(&mcu_adapter, (void *)ADDR_APP_AREA_FW);
    }
    else if(bl_state == BL_UPDATING)
    {
        ret = 1;
    }
#if (CONFIG_FW_BACKUP_AREA_ENABLE == 1)
    else if(bl_state == BL_COPYING)
    {
        ret = bl_copy_firmware();
        if(ret == 0)
        {
            //reset mcu
            mcu_reset(&mcu_adapter);
        }
    }
#endif
    
    return ret;
}



#if (CONFIG_FW_BACKUP_AREA_ENABLE == 1)
/**
  * @brief  copy fw and update boot state.
  * @param  
  * @retval 0:copy ok, other:copy fail.
  */
int bl_copy_firmware(void)
{
    int ret = -1;
    
    uint8_t buffer[STORAGE_PAGE_SIZE];
    
#if (FW_HEADER_ENABLE == 1)
    uint32_t addr_copy_form = ADDR_BACKUP_AREA_HEADER;
    uint32_t addr_copy_to = ADDR_EXE_AREA_HEADER;
    
    uint32_t copy_size = ADDR_EXE_AREA_HEADER_SIZE + ADDR_EXE_AREA_FW_SIZE;
#else
    uint32_t addr_copy_form = ADDR_BACKUP_AREA_FW;
    uint32_t addr_copy_to = ADDR_APP_AREA_FW;
    
    uint32_t copy_size = ADDR_APP_AREA_FW_SIZE;
#endif
    
    uint32_t page_num = (copy_size + STORAGE_PAGE_SIZE - 1) / STORAGE_PAGE_SIZE;
    
    while(page_num)
    {
        //earse
        ret = storage_earse_data(&storage_adapter, addr_copy_to, STORAGE_PAGE_SIZE);
        if(ret < 0)
        {
            goto exit;
        }
        
        //read
        ret = storage_read_8bit_data(&storage_adapter, addr_copy_form, buffer, STORAGE_PAGE_SIZE);
        if(ret < 0)
        {
            goto exit;
        }
        
        //write
        ret = storage_write_8bit_data(&storage_adapter, addr_copy_to, buffer, STORAGE_PAGE_SIZE);
        if(ret < 0)
        {
            goto exit;
        }
        
        page_num--;
        addr_copy_to += STORAGE_PAGE_SIZE;
        addr_copy_form += STORAGE_PAGE_SIZE;
    }
    
    //The program runs here,copy is completed,than verify fw and set boot state
#if (CONFIG_FW_VERIFY_ENABLE == 1)
    //todo:verify fw
    ret = fw_verify();
    if(ret != 0)   //verify fail
    {
        goto exit;
    }
#endif
    
    ret = bl_set_state(BL_ENTER_APP);
    if(ret < 0)
    {
        goto exit;
    }
    
exit:
    
    return ret;
}
#endif


void fw_packet_ack(bool ack)
{
    uint8_t buffer[protocol_adapter.ack_packet_max_size];
    uint32_t size;
    
    protocol_build_ack_packet(&protocol_adapter, ack, buffer, &size);
    
    if(size > 0)
    {
        com_write_data(&com_adapter, buffer, size);
    }
}



int bl_update_firmware(void)
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
        uint32_t max_write_size = ADDR_BACKUP_AREA_HEADER_SIZE + ADDR_BACKUP_AREA_FW_SIZE;
    #else
        uint32_t addr_write_to = ADDR_BACKUP_AREA_FW;
        uint32_t max_write_size = ADDR_BACKUP_AREA_FW_SIZE;
    #endif
#else
    #if (CONFIG_FW_HEADER_ENABLE == 1)
        uint32_t addr_write_to = ADDR_EXE_AREA_HEADER;
        uint32_t max_write_size = ADDR_EXE_AREA_HEADER_SIZE + ADDR_EXE_AREA_FW_SIZE;
    #else
        uint32_t addr_write_to = ADDR_EXE_AREA_FW;
        uint32_t max_write_size = ADDR_EXE_AREA_FW_SIZE;
    #endif
#endif
    
    //wait receive fw packet
    while(1)
    {
        ret = com_read_data(&com_adapter, buffer, &rce_size);
        if(ret == 0) 
        {
            ret = protocol_packet_analysis(&protocol_adapter, buffer, rce_size, &packet_att);
            if(ret == 0)
            {
                //earse
                ret = storage_earse_data(&storage_adapter, addr_write_to, packet_att.size);
                if(ret < 0)
                {
                }
                
                //write
                ret = storage_write_8bit_data(&storage_adapter, addr_write_to, packet_att.p_buffer, packet_att.size);
                if(ret < 0)
                {
                }
                
                addr_write_to += packet_att.size;
                
                //packet ack
                fw_packet_ack(true);
                
                //last packet
                if(packet_att.type == LAST_FW_PACKET_TYPE)
                {
                    
                }
            }
        }
        else    //rce err
        {
            //packet ack
            fw_packet_ack(false);
        }
    }
}


bootloader_manager_str bl_manager;

void bl_enter_app_flow(void)
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
                bl_manager.enter_app_flow_node = ENTER_APP__JUMP_APP;
                break;
            
            case ENTER_APP__VERIFY_FAIL:
                bl_manager.enter_app_flow_node = ENTER_APP__RESET;
                break;
#endif
            case ENTER_APP__JUMP_APP:
                mcu_do_boot(&mcu_adapter, (void *)ADDR_APP_AREA_FW);
                break;
            
            case ENTER_APP__RESET:
                mcu_reset(&mcu_adapter);
                break;
            default:
                break;
        }
    }
}

void bl_copy_flow(void)
{
    int ret = -1;
    
    uint8_t buffer[STORAGE_PAGE_SIZE];
    
#if (FW_HEADER_ENABLE == 1)
    uint32_t addr_copy_form = ADDR_BACKUP_AREA_HEADER;
    uint32_t addr_copy_to = ADDR_EXE_AREA_HEADER;
    
    uint32_t copy_size = ADDR_EXE_AREA_HEADER_SIZE + ADDR_EXE_AREA_FW_SIZE;
    
    bl_manager.copy_flow_node = COPY_FLOW__COPY_HEADER;
#else
    uint32_t addr_copy_form = ADDR_BACKUP_AREA_FW;
    uint32_t addr_copy_to = ADDR_APP_AREA_FW;
    
    uint32_t copy_size = ADDR_APP_AREA_FW_SIZE;
    
    bl_manager.copy_flow_node = COPY_FLOW__COPY_FW;
#endif
    
    uint32_t page_num = (copy_size + STORAGE_PAGE_SIZE - 1) / STORAGE_PAGE_SIZE;
    
    while(1)
    {
        switch(bl_manager.copy_flow_node)
        {
#if (CONFIG_FW_HEADER_ENABLE == 1)
            case COPY_FLOW__COPY_HEADER:
                break;
            case COPY_FLOW__COPY_HEADER_OK:
                break;
            case COPY_FLOW__COPY_HEADER_FAIL:
                break;
#endif
            case COPY_FLOW__COPY_FW:
                break;
            case COPY_FLOW__COPY_FW_OK:
                break;
            case COPY_FLOW__COPY_FW_FAIL:
                break;
#if (CONFIG_FW_VERIFY_ENABLE == 1)
            case COPY_FLOW__VERIFY:
                break;
            case COPY_FLOW__VERIFY_OK:
                break;
            case COPY_FLOW__VERIFY_FAIL:
                break;
#endif
            case COPY_FLOW__RESET:
                break;
            default:
                break;
        }
    }
}

void bl_update_flow(void)
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
        uint32_t max_write_size = ADDR_BACKUP_AREA_HEADER_SIZE + ADDR_BACKUP_AREA_FW_SIZE;
    
        bl_manager.update_flow_node = UPDATE_FLOW__REC_HEADER;
    #else
        uint32_t addr_write_to = ADDR_BACKUP_AREA_FW;
        uint32_t max_write_size = ADDR_BACKUP_AREA_FW_SIZE;
    
        bl_manager.update_flow_node = UPDATE_FLOW__REC_FW;
    #endif
#else
    #if (CONFIG_FW_HEADER_ENABLE == 1)
        uint32_t addr_write_to = ADDR_EXE_AREA_HEADER;
        uint32_t max_write_size = ADDR_EXE_AREA_HEADER_SIZE + ADDR_EXE_AREA_FW_SIZE;
    
        bl_manager.update_flow_node = UPDATE_FLOW__REC_HEADER;
    #else
        uint32_t addr_write_to = ADDR_EXE_AREA_FW;
        uint32_t max_write_size = ADDR_EXE_AREA_FW_SIZE;
        
        bl_manager.update_flow_node = UPDATE_FLOW__REC_FW;
    #endif
#endif
    
    while(1)
    {
        switch(bl_manager.update_flow_node)
        {
#if (CONFIG_FW_HEADER_ENABLE == 1)
            case UPDATE_FLOW__REC_HEADER:
                break;
            case UPDATE_FLOW__RCE_HEADER_OK:
                break;
            case UPDATE_FLOW__RCE_HEADER_FAIL:
                break;
#endif
            case UPDATE_FLOW__REC_FW:
                break;
            case UPDATE_FLOW__RCE_FW_OK:
                break;
            case UPDATE_FLOW__RCE_FW_FAIL:
                break;
#if (CONFIG_FW_VERIFY_ENABLE == 1)
            case UPDATE_FLOW__VERIFY:
                break;
            case UPDATE_FLOW__VERIFY_OK:
                break;
            case UPDATE_FLOW__VERIFY_FAIL:
                break;
#endif
            case UPDATE_FLOW__RESET:
                break;
            default:
                break;
        }
    }
}

void bl_main(void)
{
    uint32_t bl_state = -1;
    
    bl_get_state((uint8_t *)bl_state);
    
    switch(bl_state)
    {
        case BL_ENTER_APP:
            bl_enter_app_flow();
            break;
        case BL_COPYING:
            bl_copy_flow();
            break;
        case BL_UPDATING:
            bl_update_flow();
            break;
        default:
            break;
    }
}



