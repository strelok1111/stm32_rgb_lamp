//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#include "diag/Trace.h"

// ----------------------------------------------------------------------------
//
// Standalone STM32F1 empty sample (trace via NONE).
//
// Trace support is enabled by adding the TRACE macro definition.
// By default the trace messages are forwarded to the NONE output,
// but can be rerouted to any device or completely suppressed, by
// changing the definitions required in system/src/diag/trace_impl.c
// (currently OS_USE_TRACE_ITM, OS_USE_TRACE_SEMIHOSTING_DEBUG/_STDOUT).
//

// ----- main() ---------------------------------------------------------------

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

#define REGISTERS_COUNT 2
#define CHA_PER_REGISTER 8
#define CHA_COUNT 48
#define MAX_COUNTER 255
#define LED_COUNT 16

#define TIMER_PRESCALER 2600

#define WIFI_BAUDRATE 115200

volatile uint8_t color_data[CHA_COUNT];
volatile uint8_t global_counter = 0;

#define RX_BUFFER_SIZE 300
#define CR 0x0D
#define LF 0x0A
volatile uint8_t rx_buffer[RX_BUFFER_SIZE];
volatile uint16_t rx_wr_index=0,rx_rd_index=0;
volatile uint16_t rx_counter=0;
volatile uint8_t rx_buffer_overflow=0;
volatile uint8_t rx_last=0;
volatile uint8_t rx_last_last=0;


void touch_register_clock(void){
	GPIOB->BSRR = GPIO_Pin_13;
	GPIOB->BRR = GPIO_Pin_13;
}
void touch_register_latch(void){
	GPIOB->BSRR = GPIO_Pin_12;
	GPIOB->BRR = GPIO_Pin_12;
}

void write_data_channel_3(uint8_t data){
	if(data)
		GPIOB->BSRR = GPIO_Pin_14;
	else
		GPIOB->BRR = GPIO_Pin_14;
}
void write_data_channel_2(uint8_t data){
	if(data)
		GPIOB->BSRR = GPIO_Pin_15;
	else
		GPIOB->BRR = GPIO_Pin_15;
}
void write_data_channel_1(uint8_t data){
	if(data)
		GPIOA->BSRR = GPIO_Pin_11;
	else
		GPIOA->BRR = GPIO_Pin_11;
}

void set_led_color(uint8_t led_index,uint8_t red,uint8_t green,uint8_t blue){
	color_data[led_index * 3] = red;
	color_data[led_index * 3 + 1] = green;
	color_data[led_index * 3 + 2] = blue;
}

void registers_gpio_init(void){
	 GPIO_InitTypeDef gpio;
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	 GPIO_StructInit(&gpio);
	 gpio.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_14 | GPIO_Pin_13  | GPIO_Pin_15;
	 gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	 gpio.GPIO_Speed = GPIO_Speed_50MHz;
	 GPIO_Init(GPIOB, &gpio);

	 GPIO_InitTypeDef gpioa;
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	 GPIO_StructInit(&gpioa);
	 gpioa.GPIO_Pin = GPIO_Pin_11;
	 gpioa.GPIO_Mode = GPIO_Mode_Out_PP;
	 gpioa.GPIO_Speed = GPIO_Speed_50MHz;
	 GPIO_Init(GPIOA, &gpioa);

	 GPIO_WriteBit(GPIOA, GPIO_Pin_11,Bit_RESET);
	 GPIO_WriteBit(GPIOB, GPIO_Pin_15,Bit_RESET);
	 GPIO_WriteBit(GPIOB, GPIO_Pin_14,Bit_RESET);
	 GPIO_WriteBit(GPIOB, GPIO_Pin_12,Bit_RESET);
	 GPIO_WriteBit(GPIOB, GPIO_Pin_13,Bit_RESET);
}
void TIM2_IRQHandler(void){
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET){
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		for(uint8_t i = 0; i < 16; i++){
			write_data_channel_1(color_data[i] > global_counter);
			write_data_channel_2(color_data[i + 16] > global_counter);
			write_data_channel_3(color_data[i + 32] > global_counter);
			touch_register_clock();
		}
		touch_register_latch();
		++global_counter;
	}
}

void init_PWM_timer(void){
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	TIM_TimeBaseInitTypeDef TIM_InitStructure;
	TIM_TimeBaseStructInit(&TIM_InitStructure);
	TIM_InitStructure.TIM_Prescaler = TIMER_PRESCALER;
	TIM_InitStructure.TIM_Period = 1;
	TIM_TimeBaseInit(TIM2, &TIM_InitStructure);
	TIM_Cmd(TIM2, ENABLE);

	NVIC_EnableIRQ(TIM2_IRQn);
	TIM_ITConfig(TIM2, TIM_DIER_UIE, ENABLE);
}
void init_USART1(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	GPIO_InitTypeDef port;
	GPIO_StructInit(&port);
	port.GPIO_Mode = GPIO_Mode_AF_PP;
	port.GPIO_Pin = GPIO_Pin_9;
	port.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &port);

	port.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	port.GPIO_Pin = GPIO_Pin_10;
	port.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &port);


	USART_InitTypeDef usart;
    USART_StructInit(&usart);
    usart.USART_BaudRate = WIFI_BAUDRATE;
    USART_Init(USART1, &usart);
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART1, ENABLE);
	NVIC_EnableIRQ(USART1_IRQn);
}
void USART1_IRQHandler(){
    if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET){
    	rx_buffer[rx_wr_index]=(uint8_t)(USART_ReceiveData(USART1)& 0xFF);
    	if (rx_wr_index != 0){
    		rx_last = rx_buffer[rx_wr_index];
    		rx_last_last = rx_buffer[rx_wr_index - 1];
    	}
    	rx_wr_index++;
    	if(rx_last == LF && rx_last_last == CR){
    		//trace_printf("data: %s \r\n", rx_buffer);
    		//memset(rx_buffer,0,RX_BUFFER_SIZE);
    		rx_last_last = 0;
    		rx_last = 0;
    		rx_wr_index=0;
    	}
		if (rx_wr_index == RX_BUFFER_SIZE){
			rx_wr_index=0;
			rx_last_last = 0;
			rx_last = 0;
			memset(rx_buffer,0,RX_BUFFER_SIZE);
		}
    }
}
void send_str(USART_TypeDef* USARTx,char* str){
	char *s;
	s = str;
	while(*s){
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
		USART_SendData(USARTx, *s++);
	}
}

int main(int argc, char* argv[]){

	 registers_gpio_init();
	 timer_start();
	 init_PWM_timer();
	 init_USART1();
	 send_str(USART1,"AT+CWMODE=1\r\n");
	 timer_sleep(1000);
	 trace_printf("data: %s \r\n", rx_buffer);
	 send_str(USART1,"AT+CIPSTA=\"172.16.1.105\"\r\n");
	 timer_sleep(1000);
	 trace_printf("data: %s \r\n", rx_buffer);
	 send_str(USART1,"AT+CWJAP=\"TP-LINK_279486\",\"feliform\"\r\n");
	 timer_sleep(1000);
	 trace_printf("data: %s \r\n", rx_buffer);
	 send_str(USART1,"AT+CIPMUX=1\r\n");
	 timer_sleep(1000);
	 trace_printf("data: %s \r\n", rx_buffer);
	 send_str(USART1,"AT+CIPSERVER=1,3333\r\n");
	 while(1){
		 timer_sleep(1000);
//		 send_str(USART1,"AT\r\n");
//		 int delay = 1;
//		 for(uint16_t j = 0; j <= MAX_COUNTER; j++){
//			 for(uint8_t i = 0; i < LED_COUNT; ++i){
//				 set_led_color(i,j,0,0);
//			 }
//			 timer_sleep(delay);
//		 }
//		 trace_printf("data: %s \r\n", rx_buffer);
//		 for(uint16_t j = MAX_COUNTER+1; j > 0; j--){
//			 for(uint8_t i = 0; i < LED_COUNT; ++i){
//				 set_led_color(i,j-1,0,0);
//			 }
//			 timer_sleep(delay);
//		 }
//		 for(uint16_t j = 0; j <= MAX_COUNTER; j++){
//			 for(uint8_t i = 0; i < LED_COUNT; ++i){
//				 set_led_color(i,0,j,0);
//			 }
//			 timer_sleep(delay);
//		 }
//		 for(uint16_t j = MAX_COUNTER+1; j > 0; j--){
//			 for(uint8_t i = 0; i < LED_COUNT; ++i){
//				 set_led_color(i,0,j-1,0);
//			 }
//			 timer_sleep(delay);
//		 }
//		 for(uint16_t j = 0; j <= MAX_COUNTER; j++){
//			 for(uint8_t i = 0; i < LED_COUNT; ++i){
//				 set_led_color(i,0,0,j);
//			 }
//			 timer_sleep(delay);
//		 }
//		 for(uint16_t j = MAX_COUNTER+1; j > 0; j--){
//			 for(uint8_t i = 0; i < LED_COUNT; ++i){
//				 set_led_color(i,0,0,j-1);
//			 }
//			 timer_sleep(delay);
//		 }
	 }
}
#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
