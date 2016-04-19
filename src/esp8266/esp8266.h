#include "../at/at.h"
#include <stdint-gcc.h>
#include <string.h>

#define RESPONSE_BUFFER_LENGTH 500
#define RESIVE_HEADER_BUFFER_LENGTH 10

#define RESPONSE_STATUS_OK 1
#define RESPONSE_STATUS_ERROR 2
#define RESPONSE_STATUS_FAIL 3
#define RESPONSE_STOP_CHAR ';'

#define CALLBACK_OK_INDEX 0
#define CALLBACK_ERROR_INDEX 1
#define CALLBACK_FAIL_INDEX 2
#define CALLBACK_START_0_CONNECT_INDEX 3
#define CALLBACK_START_1_CONNECT_INDEX 4
#define CALLBACK_START_2_CONNECT_INDEX 5
#define CALLBACK_START_3_CONNECT_INDEX 6
#define CALLBACK_START_4_CONNECT_INDEX 7
#define CALLBACK_END_0_CONNECT_INDEX 8
#define CALLBACK_END_1_CONNECT_INDEX 9
#define CALLBACK_END_2_CONNECT_INDEX 10
#define CALLBACK_END_3_CONNECT_INDEX 11
#define CALLBACK_END_4_CONNECT_INDEX 12
#define CALLBACK_SEND_OK_INDEX 13
#define CALLBACK_TERMINAL_START_SEND_INDEX 14
#define CALLBACK_START_DATA_HEADER_INDEX 15
#define CALLBACK_END_DATA_HEADER_INDEX 16
#define CALLBACK_BUSY_P_INDEX 17
#define CALLBACK_READY_INDEX 18

typedef struct {
	uint8_t id;
	uint8_t is_opened;
	uint8_t is_data_recive;
	uint8_t is_data_send;
	uint32_t data_length;
	uint32_t data_index;
	char* data_buffer;
} Esp8266Connect;

typedef struct {
	ATCommand at;
	void (*data_recived)(void *,Esp8266Connect *);
	void (*on_reset)(void *);
	void (*on_ok)(void *);
	void (*on_error)(void *);
	void (*on_fail)(void *);
	volatile char* sended_command;
	char response_buffer[RESPONSE_BUFFER_LENGTH];
	char resive_header_buffer[RESIVE_HEADER_BUFFER_LENGTH];
	volatile uint16_t response_index;
	volatile uint16_t resive_heade_index;
	volatile uint8_t response_status;
	volatile Esp8266Connect *recive_connect;
	volatile Esp8266Connect *send_connect;
	volatile uint8_t is_resive_header;

	volatile uint8_t is_ready;
	volatile uint8_t is_send_data;
	volatile uint8_t is_resive_data;
	volatile Esp8266Connect *closing_connect;

	char* send_buffer;
	volatile uint32_t send_length;

	volatile Esp8266Connect connects[5];
} Esp8266;

extern void esp8266_check(Esp8266*);
extern void esp8266_hard_reset(Esp8266 *);
extern void esp8266_reset(Esp8266 *);
extern void esp8266_send_data(Esp8266 *,Esp8266Connect *,char*,uint32_t);
extern void esp8266_close_connect(Esp8266 *,Esp8266Connect *);
extern void esp8266_at(Esp8266 *);
extern void esp8266_cwmode(Esp8266 *);
extern void esp8266_cipapmac(Esp8266 *);
extern void esp8266_recive_usart_byte(Esp8266 *,uint8_t);
extern void esp8266_set_cwmode_station(Esp8266*);
extern void esp8266_set_multiple_connections(Esp8266 *);
extern void esp8266_connect_to_wifi(Esp8266 *);
extern void esp8266_start_server(Esp8266 *);
extern void esp8266_init(Esp8266 *);

