#include "bootloader.h"
#include "port.h"
#include "storage_adapter.h"
#include "com_adapter.h"
#include "mcu_adapter.h"
#include "bl_config.h"

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
        mcu_do_boot(&mcu_adapter, (void *)ADDR_EXE_AREA_FW);
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
    uint32_t addr_copy_to = ADDR_EXE_AREA_FW;
    
    uint32_t copy_size = ADDR_EXE_AREA_FW_SIZE;
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



int bl_update_firmware(void)
{
    uint8_t buffer[STORAGE_PAGE_SIZE];
    uint32_t rce_size;
    
    uint8_t fw_buffer[STORAGE_PAGE_SIZE];
    uint32_t fw_size;
    
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
            ret = protocol_fw_packet_analysis(&protocol_adapter, buffer, rce_size, fw_buffer, &fw_size);
            if(ret == 0)
            {
                //write fw to storage

                //earse
                ret = storage_earse_data(&storage_adapter, addr_write_to, fw_size);
                if(ret < 0)
                {
                }
                
                //write
                ret = storage_write_8bit_data(&storage_adapter, addr_write_to, fw_buffer, fw_size);
                if(ret < 0)
                {
                }
                
                addr_write_to += fw_size;
            }
        }
        else
        {
            //rce err
            //todo
        }
    }
}



void bl_main(void)
{
    int ret = bl_init_check();
    if(ret < 0)
    {
        //reset mcu
        mcu_reset(&mcu_adapter);
    }
    
    if(ret == 1)    
    {
        bl_update_firmware();
    }
}
