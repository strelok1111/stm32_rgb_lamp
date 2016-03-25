
#include <stdio.h>
#include <stdlib.h>
#include <stdint-gcc.h>
#define RX_BUFFER_SIZE 300
#define BUFFER_SIZE 1000
#define COMPARE_BUFFER 85


typedef struct {
	void (*on_compare_callback)(void*);
	char *compare_string;
	void *sender_pointer;
	uint8_t is_disabled;
	uint16_t compare_index;
} ATResponseCompare;

typedef struct {
	void (*send_raw_funct)(char*,uint32_t);
	ATResponseCompare *compare_callbacks;
	ATResponseCompare *current_compare;
	uint8_t callbacks_count;
	uint8_t compare_buffer[COMPARE_BUFFER];
	uint16_t response_index;

} ATCommand;


extern void AT_send_command(ATCommand*,char*);
extern void AT_enable_compare(ATResponseCompare *);
extern void AT_disable_compare(ATCommand*,ATResponseCompare *);
extern void AT_init(ATCommand*);
extern void AT_add_compare_callback(ATCommand*,char*,void (*)(void *),void *);
extern void AT_send_raw_data(ATCommand*,char*,uint32_t);
