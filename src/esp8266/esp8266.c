
#include "esp8266.h"
#include <stm32f10x_gpio.h>

void esp8266_disable_callback(Esp8266 *esp,uint8_t i){
	AT_disable_compare(&(esp->at),&(esp->at.compare_callbacks[i]));
}
void esp8266_enable_callback(Esp8266 *esp,uint8_t i){
	AT_enable_compare(&(esp->at.compare_callbacks[i]));
}
uint8_t esp8266_is_resive_header(Esp8266 *esp){
	return esp->is_resive_header;
}

uint8_t esp8266_is_resive_data(Esp8266 *esp){
	return esp->is_resive_data;
}

void esp8266_clear_after_response(Esp8266 *esp){
	if(esp8266_is_resive_data(esp))
		return;
	esp8266_disable_callback(esp,0);
	esp8266_disable_callback(esp,1);
	esp8266_disable_callback(esp,2);
	memset(esp->response_buffer,0,RESPONSE_BUFFER_LENGTH);
	esp->response_index = 0;
	esp->sended_command = NULL;
	esp->is_ready = 1;
}
void esp8266_ok_response_callback(Esp8266 *esp){
	esp->is_ready = 1;
	esp->response_status = RESPONSE_STATUS_OK;
	if(esp->closing_connect){
		esp->closing_connect->is_opened = 0;
	}
	if(esp->send_connect){
		esp8266_enable_callback(esp,13);
		AT_send_raw_data(&(esp->at),esp->send_buffer,esp->send_length);
		free(esp->send_buffer);
		esp->send_length = 0;
		esp->send_connect = NULL;
	}
	esp8266_clear_after_response(esp);
}
void esp8266_error_response_callback(Esp8266 *esp){
	esp->response_status = RESPONSE_STATUS_ERROR;
	esp8266_clear_after_response(esp);
}
void esp8266_fail_response_callback(Esp8266 *esp){
	esp->response_status = RESPONSE_STATUS_FAIL;
	esp8266_clear_after_response(esp);
}
void esp8266_start_recive_data_header(Esp8266 *esp){
	esp8266_enable_callback(esp,16);
	esp->is_ready = 0;
	esp->is_resive_header = 1;
}
void esp8266_end_recive_data_header(Esp8266 *esp){
	esp->resive_heade_index = 0;
	esp->is_resive_header = 0;
	esp->is_resive_data = 1;
	esp8266_enable_callback(esp,15);
	esp8266_disable_callback(esp,16);
	char *channel_number = strtok(esp->resive_header_buffer,",");
	char *number_of_bytes_str = strtok(NULL,",");
	number_of_bytes_str[strlen(number_of_bytes_str) - 1] = 0;
	uint8_t num = atoi(channel_number);
	esp->recive_connect = &(esp->connects[num]);
	esp->recive_connect->is_data_recive = 1;
	esp->recive_connect->data_index = 0;
	esp->recive_connect->data_length = atoi(number_of_bytes_str);
	esp->recive_connect->data_buffer = malloc(esp->recive_connect->data_length);
	memset(esp->resive_header_buffer,0,RESIVE_HEADER_BUFFER_LENGTH);
}
void esp8266_end_send_data(Esp8266 *esp){
	esp8266_disable_callback(esp,13);
}
void esp8266_start_send_data(Esp8266 *esp){

}
void esp8266_start_0_connect(Esp8266 *esp){
	esp->connects[0].is_opened = 1;
	esp8266_enable_callback(esp,8);
}
void esp8266_start_1_connect(Esp8266 *esp){
	esp->connects[1].is_opened = 1;
	esp8266_enable_callback(esp,9);
}
void esp8266_start_2_connect(Esp8266 *esp){
	esp->connects[2].is_opened = 1;
	esp8266_enable_callback(esp,10);
}
void esp8266_start_3_connect(Esp8266 *esp){
	esp->connects[3].is_opened = 1;
	esp8266_enable_callback(esp,11);
}
void esp8266_start_4_connect(Esp8266 *esp){
	esp->connects[4].is_opened = 1;
	esp8266_enable_callback(esp,12);
}
void esp8266_end_0_connect(Esp8266 *esp){
	esp->connects[0].is_opened = 0;
	esp8266_disable_callback(esp,8);
}
void esp8266_end_1_connect(Esp8266 *esp){
	esp->connects[1].is_opened = 0;
	esp8266_disable_callback(esp,9);
}
void esp8266_end_2_connect(Esp8266 *esp){
	esp->connects[2].is_opened = 0;
	esp8266_disable_callback(esp,10);
}
void esp8266_end_3_connect(Esp8266 *esp){
	esp->connects[3].is_opened = 0;
	esp8266_disable_callback(esp,11);
}
void esp8266_end_4_connect(Esp8266 *esp){
	esp->connects[4].is_opened = 0;
	esp8266_disable_callback(esp,12);
}
void esp8266_busy_p(Esp8266 *esp){

}
void esp8266_ready(Esp8266 *esp){
	esp->is_ready = 1;
}
void esp8266_init(Esp8266 *esp){

	GPIO_InitTypeDef gpio;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_StructInit(&gpio);
	gpio.GPIO_Pin = GPIO_Pin_8;
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &gpio);
	GPIOA->BSRR = GPIO_Pin_8;


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

	esp8266_disable_callback(esp,0);
	esp8266_disable_callback(esp,1);
	esp8266_disable_callback(esp,2);

	esp8266_disable_callback(esp,8);
	esp8266_disable_callback(esp,9);
	esp8266_disable_callback(esp,10);
	esp8266_disable_callback(esp,11);
	esp8266_disable_callback(esp,12);
	esp8266_disable_callback(esp,13);
	esp8266_disable_callback(esp,14);
	esp8266_disable_callback(esp,16);

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
	esp->is_ready=0;
}
void esp8266_recive_usart_byte(Esp8266 *esp,uint8_t byte){
	if(esp->is_resive_data){
		esp->recive_connect->data_buffer[esp->recive_connect->data_index++] = byte;
		if(esp->recive_connect->data_index == esp->recive_connect->data_length){
			esp->is_resive_data = 0;
			esp->is_ready = 1;
			if(esp->data_recived)
				esp->data_recived(esp,esp->recive_connect);
			esp->recive_connect->is_data_recive = 0;
			free(esp->recive_connect->data_buffer);
			esp->recive_connect->data_length = 0;
			esp->recive_connect->data_index = 0;
			uint8_t conn_id = esp->recive_connect->id;
			esp->recive_connect = NULL;
		}
		return;
	}
	uint8_t is_resive_header = esp8266_is_resive_header(esp);
	if(esp->sended_command && !is_resive_header && !esp->recive_connect){
		esp->response_buffer[esp->response_index++] = byte;
	}
	if(is_resive_header){
		esp->resive_header_buffer[esp->resive_heade_index++] = byte;
	}
	AT_recive_byte(&(esp->at),byte);
}
void esp8266_send_command(Esp8266 *esp,char* sstr){
	while(!esp->is_ready);
	esp8266_enable_callback(esp,0);
	esp8266_enable_callback(esp,1);
	esp8266_enable_callback(esp,2);
	esp->sended_command = sstr;
	esp->is_ready = 0;
	AT_send_command(&(esp->at),sstr);
	//while(!esp->is_ready);
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
void esp8266_set_cwmode_station(Esp8266 *esp){
	esp8266_send_command(esp,"AT+CWMODE=1\r\n");
}
void esp8266_cwlap(Esp8266 *esp){
	esp8266_send_command(esp,"AT+CWLAP\r\n");
}
void esp8266_connect_to_wifi(Esp8266 *esp){
	esp8266_send_command(esp,"AT+CWJAP=\"TP-LINK_279486\",\"feliform\"\r\n");
}
void esp8266_set_multiple_connections(Esp8266 *esp){
	esp8266_send_command(esp,"AT+CIPMUX=1\r\n");
}
void esp8266_start_server(Esp8266 *esp){
	esp8266_send_command(esp,"AT+CIPSERVER=1,3000\r\n");
}
void esp8266_set_ip(Esp8266 *esp){
	esp8266_send_command(esp,"AT+CIPSTA=\"172.16.1.105\"\r\n");
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

	esp->send_connect = connect;
	connect->is_data_send = 1;

	esp->send_buffer = malloc(length);
	memccpy(esp->send_buffer,data,0,length);
	esp->send_length = length;

	esp8266_send_command(esp,buffer_str);
}
void esp8266_hard_reset(Esp8266 *esp){
	esp->is_ready = 0;
	GPIOA->BRR = GPIO_Pin_8;
	GPIOA->BSRR = GPIO_Pin_8;
}
