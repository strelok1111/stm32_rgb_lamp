#include "at.h"
#include <string.h>
#include <stdint-gcc.h>
char* end_strings[] = {
		"",
		"OK\r\n",
		"ERROR\r\n",
		"FAIL\r\n"
};
void AT_recive_byte(ATCommand* at,uint8_t byte){
	if(at->recive_data && at->is_bisy){

		if(!at->end_string_index && at->current_index_char_end_check == 0){
			for(uint8_t i = 1;i <= 3;i++){
				if(end_strings[i][0] == byte){
					at->end_string_index = i;
					break;
				}
			}
		}
		char *end_string = end_strings[at->end_string_index];
		if(at->end_string_index){
			if(end_string[at->current_index_char_end_check] == byte && at->current_index_char_end_check == 0){
				at->current_index_char_end_check++;
				at->start_end = 1;
			}else if(end_string[at->current_index_char_end_check] == byte && at->start_end){
				at->current_index_char_end_check++;
			}else if(end_string[at->current_index_char_end_check] != byte && at->start_end){
				at->current_index_char_end_check = 0;
				at->start_end = 0;
				at->end_string_index = 0;
			}
		}
		at->data_buffer[at->data_index++] = byte;
		if(at->current_index_char_end_check != 0 && at->current_index_char_end_check == strlen(end_string)){
			at->recive_data = 0;
			at->sended_command = "";
			at->data_index = 0;
			at->current_index_char_end_check = 0;
			at->current_index_char_check = 0;
			at->start_end = 0;
			at->start_echo = 0;
			AT_parse_command(at);
			at->end_string_index = 0;
			memset(at->data_buffer,0,BUFFER_SIZE);
			at->is_bisy = 0;
		}
	}else if(at->is_bisy){
		if(at->sended_command[at->current_index_char_check] == byte && at->current_index_char_check == 0){
			at->current_index_char_check++;
			at->start_echo = 1;
		}else if(at->sended_command[at->current_index_char_check] == byte && at->start_echo){
			at->current_index_char_check++;
		}else if(at->sended_command[at->current_index_char_check] != byte && at->start_echo){
			at->start_echo = 0;
			at->current_index_char_check = 0;
		}
		if(at->current_index_char_check == strlen(at->sended_command)){
			at->recive_data = 1;
			at->start_echo = 0;
		}
	}
}
void AT_send_command(ATCommand* at,char* str){
	if(strlen(at->sended_command))
		return;
	at->is_bisy = 1;
	at->current_index_char_check = 0;
	at->recive_data = 0;
	at->start_echo = 0;
	at->data_index = 0;
	at->current_index_char_end_check = 0;
	at->start_end = 0;
	memset(at->data_buffer,0,BUFFER_SIZE);
	at->sended_command = str;
	at->end_string_index = 0;
	at->send_funct(str);
	at->send_funct("\r\n");
}
void AT_parse_command(ATCommand* at){
	char* buffer[BUFFER_SIZE];
	memcpy(buffer,at->data_buffer,BUFFER_SIZE);

}
