#include "port.h"
#include "user_adapter.h"

com_adapter_str com_adapter = 
{
   .p_read_data = uart_read_data,
   .p_write_data = uart_write_data,
};


storage_adapter_str storage_adapter = 
{
   .p_read_data = flash_read_data,
   .p_write_data = flash_write_data,
   .p_earse_data = flash_earse_data,
};


mcu_adapter_str mcu_adapter = 
{
    .p_do_boot = jump_to_app,
    .p_reset = nvic_reset,
};


protocol_adapter_str protocol_adapter = 
{
    .p_packet_analysis = packet_analysis,
};


bootloader_callback_str bl_callback;

