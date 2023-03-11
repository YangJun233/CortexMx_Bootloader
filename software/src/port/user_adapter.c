#include "user_adapter.h"
#include "usart.h"

/***********************************************************\
 * uart
 */
static volatile uint16_t uart_read_size;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE) != RESET) //空闲中断判断   __HAL_UART_GET_FLAG获取指定中断
	{
		__HAL_UART_CLEAR_IDLEFLAG(huart);//清除空间中断

		HAL_UART_DMAStop(huart);//停止DMA
		uart_read_size = 256 - __HAL_DMA_GET_COUNTER(huart->hdmarx);//获取已接收长度
	}
}


int uart_read_data(uint8_t * p, uint32_t * p_size)
{
    uart_read_size = 0;

    HAL_UART_Receive_DMA(&huart1, p, 256);//开启接收

    while(uart_read_size == 0);

    *p_size = uart_read_size;

    return 0;
}


int uart_write_data(uint8_t * p, uint32_t size)
{
    HAL_UART_Transmit_DMA(&huart1, p, size);
}



/***********************************************************\
 * mcu 
 */
int jump_to_app(uint32_t addr)
{
    typedef void (*iapfun)(void);
    iapfun jump2app;

    if(((*(__IO uint32_t*)addr)&0x2FFE0000)==0x20000000)	//检查栈顶地址是否合法.
	{ 
        __set_FAULTMASK(1);                                 //关闭中断
		jump2app = (iapfun)*(__IO uint32_t*)(addr+4);		//用户代码区第二个字为程序开始地址(复位地址)		
		__set_MSP(*(__IO uint32_t*)addr);					//初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址)
		jump2app();									        //跳转到APP.
	}
    else
    {
        return -1;
    }

    return 0;
}

void nvic_reset(void)
{
    HAL_NVIC_SystemReset();
}



/***********************************************************\
 * protocol
 */
int packet_analysis(uint8_t * p_in, uint32_t size_in, uint8_t * type, uint8_t * p_out, uint32_t * p_size_out)
{

}



/***********************************************************\
 * flash 
 */
#define ONCHIP_FLASH_SIZE                   ((uint32_t)(128 * 1024)) 
#define ONCHIP_FLASH_END_ADDRESS            ((uint32_t)(FLASH_BASE + ONCHIP_FLASH_SIZE))            /* 片内flash末地址 */
int flash_read_data(uint32_t addr, uint8_t * p, uint32_t size)
{
    if ((addr + size) > ONCHIP_FLASH_END_ADDRESS)
    {
        return -1;
    }

    for (uint32_t i = 0; i < size; i++)
    {
        *p = *(uint8_t *) addr;
        p++;
        addr++;
    }

    return 0;
}


/**
 * @brief  
 * @note   
 * @param[in]  offset: 偏移地址 
 * @param[in]  buf: 数据池
 * @param[in]  size: 数据长度，单位 byte
 * @retval -1: 失败。 0: 成功
 */
int flash_write_data(uint32_t addr, uint8_t * p, uint32_t size)
{
    uint8_t ret = 0;
    uint32_t  byte_step = 0;
    uint8_t index = 0;
    int8_t  status = 0;
    uint8_t byte_num = sizeof(uint32_t);

    uint32_t write_data = 0, temp_data = 0;

    if ((addr + size) > ONCHIP_FLASH_END_ADDRESS)
    {
        return -1;
    }
    
    if (size < 1)
    {
        return -1;
    }

    HAL_FLASH_Unlock();

    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);
    
    byte_num = sizeof(uint32_t);
    
    for (byte_step = 0; byte_step < size; )
    {
        if ((size - byte_step) < byte_num)         /* The number of bytes less than a WORD/half WORD */
        {
            for (index = 0; (size - byte_step) > 0; index++)
            {
                temp_data = *p;
                write_data = (write_data) | (temp_data << 8*index);
                
                p++;
                byte_step++;
            }
        }
        else
        {
            for (index = 0; index < byte_num; index++)
            {
                temp_data = *p;
                write_data = (write_data) | (temp_data << 8*index);
                
                p++;
                byte_step++;
            }
        }

        /* write data */
        ret = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, write_data);
        if (HAL_OK == ret)
        {
            /* Check the written value */
            if (*(uint32_t *)addr != write_data)
            {
                status = -1;
                goto __exit;
            }
        }
        else
        {
            status = -1;
            goto __exit;
        }
        
        temp_data = 0;
        write_data = 0;

        addr += byte_num;
    }

__exit:
    HAL_FLASH_Lock();
    
    if (status)
    {
        return status;
    }

    return size;
}


/**
 * @brief  
 * @note   
 * @param[in]   offset: 偏移地址 
 * @param[in]   size: 数据长度，单位 byte
 * @retval -2: 擦除失败。 -1: 超过 flash 大小。 0: 成功 
 */
int flash_earse_data(uint32_t addr, uint32_t size)
{
    uint32_t PageError = 0;
    HAL_StatusTypeDef status = HAL_OK;
    FLASH_EraseInitTypeDef pEraseInit;
    uint32_t end_addr = addr + size;

    if (end_addr > ONCHIP_FLASH_END_ADDRESS)
    {
        return -1;
    }

    HAL_FLASH_Unlock();
    
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);
    
    pEraseInit.TypeErase   = FLASH_TYPEERASE_PAGES;
    pEraseInit.PageAddress = addr;
    pEraseInit.Banks       = FLASH_BANK_1;
    pEraseInit.NbPages     = 1;

    for (;
         pEraseInit.PageAddress < end_addr;
         pEraseInit.PageAddress += FLASH_PAGE_SIZE)
    {
        status = HAL_FLASHEx_Erase(&pEraseInit, &PageError);
        if (status != HAL_OK)
        {
            break;
        }
    }

    HAL_FLASH_Lock();
    
    if (status != HAL_OK)
    {
        return -2;
    }

    return size;
}
