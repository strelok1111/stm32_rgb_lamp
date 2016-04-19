
#include "esp8266.h"
#include <stm32f10x_gpio.h>
void esp8266_hard_reset(Esp8266 *esp){
	esp -> is_ready = 0;
	GPIOA -> BRR = GPIO_Pin_8;
	GPIOA -> BSRR = GPIO_Pin_8;
}
void esp8266_set_on_ok_callback(Esp8266 *esp,void (*callback)(void*)){
	esp -> on_ok = callback;
}
void esp8266_set_on_error_callback(Esp8266 *esp,void (*callback)(void*)){
	esp -> on_error = callback;
}
void esp8266_set_on_fail_callback(Esp8266 *esp,void (*callback)(void*)){
	esp -> on_fail = callback;
}
void esp8266_reset(Esp8266 *esp){
	esp -> is_ready = 0;
	AT_send_command(&(esp->at),"AT+RST\r\n");
}
void esp8266_disable_callback(Esp8266 *esp,uint8_t i){
	AT_disable_compare(&(esp->at),&(esp->at.compare_callbacks[i]));
}
void esp8266_enable_callback(Esp8266 *esp,uint8_t i){
	AT_enable_compare(&(esp->at.compare_callbacks[i]));
}

void esp8266_clear_after_response(Esp8266 *esp){
	esp8266_disable_callback(esp,CALLBACK_OK_INDEX);
	esp8266_disable_callback(esp,CALLBACK_ERROR_INDEX);
	esp8266_disable_callback(esp,CALLBACK_FAIL_INDEX);
	memset(esp->response_buffer,0,RESPONSE_BUFFER_LENGTH);
	esp -> response_index = 0;
	esp -> sended_command = NULL;
	esp -> is_ready = 1;
}
void esp8266_ok_response_callback(Esp8266 *esp){
	esp->response_status = RESPONSE_STATUS_OK;
	esp8266_clear_after_response(esp);
	if(esp -> on_ok)
		esp -> on_ok(esp);
}
void esp8266_error_response_callback(Esp8266 *esp){
	esp->response_status = RESPONSE_STATUS_ERROR;
	esp8266_clear_after_response(esp);
	if(esp -> on_error)
		esp -> on_error(esp);
}
void esp8266_fail_response_callback(Esp8266 *esp){
	esp->response_status = RESPONSE_STATUS_FAIL;
	esp8266_clear_after_response(esp);
	if(esp -> on_fail)
		esp -> on_fail(esp);
}
void esp8266_start_recive_data_header(Esp8266 *esp){
	esp->is_ready = 0;
	esp->resive_heade_index = 0;
	esp->is_resive_data = 0;
	esp->is_resive_header = 1;
	esp->recive_connect = NULL;
}
void esp8266_end_recive_data_header(Esp8266 *esp){
	esp->resive_heade_index = 0;
	esp->is_resive_header = 0;
	esp->is_resive_data = 1;
	uint8_t num = atoi(strtok(esp->resive_header_buffer,","));
	esp->recive_connect = &(esp->connects[num]);
	esp->recive_connect->is_data_recive = 1;
	esp->recive_connect->data_index = 0;
	esp->recive_connect->data_length = atoi(strtok(NULL,","));
	esp->recive_connect->data_buffer = malloc(esp->recive_connect->data_length);
	memset(esp->resive_header_buffer,0,RESIVE_HEADER_BUFFER_LENGTH);
}
void esp8266_end_send_data(Esp8266 *esp){
	esp -> is_send_data = 0;
	esp8266_disable_callback(esp,CALLBACK_SEND_OK_INDEX);
}
void esp8266_start_send_data(Esp8266 *esp){
	esp8266_disable_callback(esp,CALLBACK_TERMINAL_START_SEND_INDEX);
	esp8266_enable_callback(esp,CALLBACK_SEND_OK_INDEX);
	AT_send_raw_data(&(esp->at),esp -> send_buffer,esp -> send_length);
}
void esp8266_start_0_connect(Esp8266 *esp){
	esp->connects[0].is_opened = 1;
	esp8266_enable_callback(esp,CALLBACK_END_0_CONNECT_INDEX);
}
void esp8266_start_1_connect(Esp8266 *esp){
	esp->connects[1].is_opened = 1;
	esp8266_enable_callback(esp,CALLBACK_END_1_CONNECT_INDEX);
}
void esp8266_start_2_connect(Esp8266 *esp){
	esp->connects[2].is_opened = 1;
	esp8266_enable_callback(esp,CALLBACK_END_2_CONNECT_INDEX);
}
void esp8266_start_3_connect(Esp8266 *esp){
	esp->connects[3].is_opened = 1;
	esp8266_enable_callback(esp,CALLBACK_END_3_CONNECT_INDEX);
}
void esp8266_start_4_connect(Esp8266 *esp){
	esp->connects[4].is_opened = 1;
	esp8266_enable_callback(esp,CALLBACK_END_4_CONNECT_INDEX);
}
void esp8266_end_0_connect(Esp8266 *esp){
	esp->connects[0].is_opened = 0;
	esp8266_disable_callback(esp,CALLBACK_END_0_CONNECT_INDEX);
}
void esp8266_end_1_connect(Esp8266 *esp){
	esp->connects[1].is_opened = 0;
	esp8266_disable_callback(esp,CALLBACK_END_1_CONNECT_INDEX);
}
void esp8266_end_2_connect(Esp8266 *esp){
	esp->connects[2].is_opened = 0;
	esp8266_disable_callback(esp,CALLBACK_END_2_CONNECT_INDEX);
}
void esp8266_end_3_connect(Esp8266 *esp){
	esp->connects[3].is_opened = 0;
	esp8266_disable_callback(esp,CALLBACK_END_3_CONNECT_INDEX);
}
void esp8266_end_4_connect(Esp8266 *esp){
	esp->connects[4].is_opened = 0;
	esp8266_disable_callback(esp,CALLBACK_END_4_CONNECT_INDEX);
}
void esp8266_busy_p(Esp8266 *esp){

}
void esp8266_ready(Esp8266 *esp){
	esp->is_ready = 1;
	if(esp -> on_reset)
		esp-> on_reset(esp);
}
void esp8266_init(Esp8266 *esp){

	GPIO_InitTypeDef gpio;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_StructInit(&gpio);
	gpio.GPIO_Pin = GPIO_Pin_8;
	gpio.GPIO_Mode = GPIO_Mode_IPD;
	gpio.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &gpio);

	AT_init(&(esp->at));
	AT_add_compare_callback(&(esp->at),"OK\r\n",esp8266_ok_response_callback,esp);      //0
	AT_add_compare_callback(&(esp->at),"ERROR\r\n",esp8266_error_response_callback,esp);//1
	AT_add_compare_callback(&(esp->at),"FAIL\r\n",esp8266_fail_response_callback,esp);  //2

	AT_add_compare_callback(&(esp->at),"0,CONNECT\r\n",esp8266_start_0_connect,esp);		//3
	AT_add_compare_callback(&(esp->at),"1,CONNECT\r\n",esp8266_start_1_connect,esp);		//4
	AT_add_compare_callback(&(esp->at),"2,CONNECT\r\n",esp8266_start_2_connect,esp);		//5
	AT_add_compare_callback(&(esp->at),"3,CONNECT\r\n",esp8266_start_3_connect,esp);		//6
	AT_add_compare_callback(&(esp->at),"4,CONNECT\r\n",esp8266_start_4_connect,esp);		//7
	AT_add_compare_callback(&(esp->at),"0,CLOSED",esp8266_end_0_connect,esp);			//8
	AT_add_compare_callback(&(esp->at),"1,CLOSED",esp8266_end_1_connect,esp);			//9
	AT_add_compare_callback(&(esp->at),"2,CLOSED",esp8266_end_2_connect,esp);			//10
	AT_add_compare_callback(&(esp->at),"3,CLOSED",esp8266_end_3_connect,esp);			//11
	AT_add_compare_callback(&(esp->at),"4,CLOSED",esp8266_end_4_connect,esp);			//12

	AT_add_compare_callback(&(esp->at),"SEND OK\r\n",esp8266_end_send_data,esp);		//13
	AT_add_compare_callback(&(esp->at),"> ",esp8266_start_send_data,esp);				//14
	AT_add_compare_callback(&(esp->at),"+IPD,",esp8266_start_recive_data_header,esp);	//15
	AT_add_compare_callback(&(esp->at),":",esp8266_end_recive_data_header,esp);			//16

	AT_add_compare_callback(&(esp->at),"busy p...\r\n",esp8266_busy_p,esp);             //17
	AT_add_compare_callback(&(esp->at),"ready\r\n",esp8266_ready,esp);                  //18

	esp8266_disable_callback(esp,CALLBACK_OK_INDEX);
	esp8266_disable_callback(esp,CALLBACK_ERROR_INDEX);
	esp8266_disable_callback(esp,CALLBACK_FAIL_INDEX);

	esp8266_disable_callback(esp,CALLBACK_END_0_CONNECT_INDEX);
	esp8266_disable_callback(esp,CALLBACK_END_1_CONNECT_INDEX);
	esp8266_disable_callback(esp,CALLBACK_END_2_CONNECT_INDEX);
	esp8266_disable_callback(esp,CALLBACK_END_3_CONNECT_INDEX);
	esp8266_disable_callback(esp,CALLBACK_END_4_CONNECT_INDEX);
	esp8266_disable_callback(esp,CALLBACK_SEND_OK_INDEX);
	esp8266_disable_callback(esp,CALLBACK_TERMINAL_START_SEND_INDEX);
	esp8266_disable_callback(esp,CALLBACK_END_DATA_HEADER_INDEX);

	esp->connects[0].id = 0;
	esp->connects[1].id = 1;
	esp->connects[2].id = 2;
	esp->connects[3].id = 3;
	esp->connects[4].id = 4;
	memset(esp->response_buffer,0,RESPONSE_BUFFER_LENGTH);
	memset(esp->resive_header_buffer,0,RESIVE_HEADER_BUFFER_LENGTH);
	esp->response_index = 0;
	esp->resive_heade_index = 0;
	esp->sended_command = NULL;
	esp->recive_connect = NULL;
	esp->is_resive_header = 0;
	esp->is_resive_data = 0;
	esp->closing_connect=NULL;
	esp->is_ready = 0;
	GPIOA->BSRR = GPIO_Pin_8;
	esp8266_reset(esp);
}
void esp8266_recive_usart_byte(Esp8266 *esp,uint8_t byte){
	if(esp->recive_connect){
		esp->recive_connect->data_buffer[esp->recive_connect->data_index++] = byte;
		if(esp->recive_connect->data_index == esp->recive_connect->data_length){
			esp8266_disable_callback(esp,CALLBACK_END_DATA_HEADER_INDEX);
			if(esp->data_recived)
				esp->data_recived(esp,esp->recive_connect);
			esp->recive_connect->is_data_recive = 0;
			esp->is_ready = 1;
			esp->is_resive_data = 0;
			esp->recive_connect->data_length = 0;
			esp->recive_connect->data_index = 0;
			free(esp->recive_connect->data_buffer);
			esp->recive_connect = NULL;
		}
	}else{
		if(esp -> is_resive_header && byte != ':'){
			esp->resive_header_buffer[esp->resive_heade_index++] = byte;
		}else if(esp -> is_resive_header && byte == ':'){
			esp8266_end_recive_data_header(esp);
		}
		AT_recive_byte(&(esp->at),byte);
	}
}

void esp8266_send_command_with_callback(Esp8266 *esp,char* sstr,void(*ok_callback)(void*),void(*fail_callback)(void*),void(*error_callback)(void*)){
	esp8266_enable_callback(esp,CALLBACK_OK_INDEX);
	esp8266_enable_callback(esp,CALLBACK_ERROR_INDEX);
	esp8266_enable_callback(esp,CALLBACK_FAIL_INDEX);
	esp->sended_command = sstr;
	esp->is_ready = 0;
	esp8266_set_on_ok_callback(esp,ok_callback);
	esp8266_set_on_fail_callback(esp,fail_callback);
	esp8266_set_on_error_callback(esp,error_callback);
	AT_send_command(&(esp->at),sstr);
}
void esp8266_at(Esp8266 *esp){
	esp8266_send_command(esp,"AT\r\n");
}
void esp8266_cwmode(Esp8266 *esp){
	esp8266_send_command(esp,"AT+CWMODE?\r\n");
}
void esp8266_cipapmac(Esp8266 *esp){
	esp8266_send_command(esp,"AT+CIPAPMAC?\r\n");
}
void esp8266_gmr(Esp8266 *esp){
	esp8266_send_command(esp,"AT+GMR\r\n");
}

void esp8266_cwlap(Esp8266 *esp){
	esp8266_send_command(esp,"AT+CWLAP\r\n");
}

void esp8266_close_connect(Esp8266 *esp,Esp8266Connect *connect){
	return ; //Не работает на прошивке 0.21 баг в модуле!
	char buffer_str[100];
	char channel_number[1];
	itoa(connect->id,channel_number,10);
	memset(buffer_str,0,100);
	strcat(buffer_str,"AT+CIPCLOSE=");
	strcat(buffer_str,channel_number);
	strcat(buffer_str,"\r\n");
	esp8266_send_command(esp,buffer_str);
	esp->closing_connect = connect;
}
void esp8266_send_data(Esp8266 *esp,Esp8266Connect *connect,char *data,uint32_t length){
	char buffer_str[100];
	char channel_number[1];
	char length_buffer[5];
	itoa(connect->id,channel_number,10);
	itoa(length,length_buffer,10);
	memset(buffer_str,0,100);
	strcat(buffer_str,"AT+CIPSEND=");
	strcat(buffer_str,channel_number);
	strcat(buffer_str,",");
	strcat(buffer_str,length_buffer);
	strcat(buffer_str,"\r\n");

	esp -> send_connect = connect;
	connect -> is_data_send = 1;
	esp -> is_ready = 0;
	esp -> is_send_data = 1;

	esp->send_buffer = malloc(length);
	memcpy(esp->send_buffer,data,length);
	esp->send_length = length;
	esp8266_enable_callback(esp,CALLBACK_TERMINAL_START_SEND_INDEX);
	AT_send_command(&(esp->at),buffer_str);
}

