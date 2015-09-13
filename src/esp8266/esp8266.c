
#include "esp8266.h"
#include <stdint-gcc.h>
void esp8266_recive_usart_byte(Esp8266 *esp,uint8_t byte){
	AT_recive_byte(&(esp->at),byte);
}
void esp8266_send_command(Esp8266 *esp,char* comm){
	while((esp->at).is_bisy){};
	AT_send_command(&(esp->at),comm);
}
void esp8266_at(Esp8266 *esp){
	esp8266_send_command(esp,"AT");
}
void esp8266_cwmode(Esp8266 *esp){
	esp8266_send_command(esp,"AT+CWMODE?");
}
void esp8266_cipapmac(Esp8266 *esp){
	esp8266_send_command(esp,"AT+CIPAPMAC?");
}
void esp8266_gmr(Esp8266 *esp){
	esp8266_send_command(esp,"AT+GMR");
}
void esp8266_set_cwmode_station(Esp8266 *esp){
	esp8266_send_command(esp,"AT+CWMODE=1");
}
void esp8266_cwlap(Esp8266 *esp){
	esp8266_send_command(esp,"AT+CWLAP");
}
