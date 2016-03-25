#include "at.h"
#include <string.h>
#include <stdint-gcc.h>
void AT_recive_byte(ATCommand* at,uint8_t byte){
	if(at->current_compare && !at->current_compare->is_disabled){
		if(at->current_compare->compare_string[at->current_compare->compare_index] == byte){
			at->current_compare->compare_index++;
		}else{
			at->current_compare->compare_index = 0;
			at->current_compare = NULL;
		}

	}else{
		for(uint8_t i = 0;i < at->callbacks_count;i++){
			if(at->compare_callbacks[i].compare_string[0] == byte && !at->compare_callbacks[i].is_disabled){
				at->current_compare = &(at->compare_callbacks[i]);
				at->current_compare->compare_index = 1;
			}
		}
	}
	if(at->current_compare && at->current_compare->compare_index == strlen(at->current_compare->compare_string)){
		if(at->current_compare->on_compare_callback){
			at->current_compare->compare_index = 0;
			void (*comp)(void*) = at->current_compare->on_compare_callback;
			void *sender = at->current_compare->sender_pointer;
			at->current_compare = NULL;
			comp(sender);
		}
	}
}
void AT_send_command(ATCommand* at,char* str){
	AT_send_raw_data(at,str,strlen(str));
}
void AT_send_raw_data(ATCommand* at,char* str,uint32_t length){
	at->send_raw_funct(str,length);
}
void AT_init(ATCommand* at){
	at->callbacks_count = 0;
	at->response_index = 0;
}
void AT_add_compare_callback(ATCommand* at,char* compare_string,void (*callback)(void*),void *object_pointer){
	ATResponseCompare new_response_struct;
	new_response_struct.compare_string = compare_string;
	new_response_struct.compare_index = 0;
	new_response_struct.is_disabled = 0;
	new_response_struct.on_compare_callback = callback;
	new_response_struct.sender_pointer = object_pointer;
	at->callbacks_count++;
	ATResponseCompare *compares = realloc(at->compare_callbacks, sizeof(ATResponseCompare) * at->callbacks_count);
	if(compares){
		at->compare_callbacks = compares;
		at->compare_callbacks[at->callbacks_count - 1] = new_response_struct;
	}
}
void AT_disable_compare(ATCommand* at,ATResponseCompare *compare){
	if(compare == at->current_compare)
		at->current_compare = NULL;
	compare->is_disabled = 1;
	compare->compare_index = 0;
}
void AT_enable_compare(ATResponseCompare *compare){
	compare->is_disabled = 0;
}
