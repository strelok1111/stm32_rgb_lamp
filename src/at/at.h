
#include <stdio.h>
#include <stdlib.h>
#include <stdint-gcc.h>
#define RX_BUFFER_SIZE 300
#define CR 0x0D
#define LF 0x0A
#define BUFFER_SIZE 1000
//volatile uint8_t rx_buffer[RX_BUFFER_SIZE];
//volatile uint16_t rx_wr_index=0,rx_rd_index=0;
//volatile uint16_t rx_counter=0;
//volatile uint8_t rx_buffer_overflow=0;
//volatile uint8_t rx_last=0;
//volatile uint8_t rx_last_last=0;

typedef struct {
	void (*send_funct)(char*);
	char* sended_command;
	uint16_t current_index_char_check;
	uint8_t start_echo;
	uint8_t recive_data;
	uint8_t current_index_char_end_check;
	uint8_t start_end;
	uint16_t data_index;
	uint8_t end_string_index;
	uint8_t is_bisy;
	char data_buffer[BUFFER_SIZE];
} ATCommand;

extern void AT_send_command(ATCommand*,char*);
