#ifndef USER_ADAPTER_H
#define USER_ADAPTER_H

#include <stdint.h>

int uart_read_data(uint8_t * p, uint32_t * p_size);
int uart_write_data(uint8_t * p, uint32_t size);

int flash_read_data(uint32_t addr, uint8_t * p, uint32_t size);
int flash_write_data(uint32_t addr, uint8_t * p, uint32_t size);
int flash_earse_data(uint32_t addr, uint32_t size);

int jump_to_app(uint32_t addr);
void nvic_reset(void);

int packet_analysis(uint8_t * p_in, uint32_t size_in, uint8_t * type, uint8_t * p_out, uint32_t * p_size_out);

#endif
