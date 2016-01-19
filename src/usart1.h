#include <stm32f10x_gpio.h>
#define BAUDRATE 115200
extern void send_data_USART_wifi(char*);
extern void send_data_raw_USART_wifi(char*,uint32_t);
extern void init_USART_wifi(void);
