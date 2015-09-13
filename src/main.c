//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>

#include <stm32f10x_rcc.h>
#include "diag/Trace.h"
#include "led_register/led_register.h"
#include "esp8266/esp8266.h"
#include "usart1.h"

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

Esp8266 esp;

void TIM2_IRQHandler(void){
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET){
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		register_timer_tick();
	}
}


void USART1_IRQHandler(){
    if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET){
    	esp8266_recive_usart_byte(&esp,(uint8_t)(USART_ReceiveData(USART1)& 0xFF));
    }
}

int main(int argc, char* argv[]){

	 registers_gpio_init();
	 register_init_timer2();
	 timer_start();
	 init_USART_wifi();
	 esp.at.send_funct = send_data_USART_wifi;
	 esp8266_set_cwmode_station(&esp);

//	 send_str(USART1,"AT+CWMODE=1\r\n");
//	 timer_sleep(1000);
//	 trace_printf("data: %s \r\n", rx_buffer);
//	 send_str(USART1,"AT+CIPSTA=\"172.16.1.105\"\r\n");
//	 timer_sleep(1000);
//	 trace_printf("data: %s \r\n", rx_buffer);
//	 send_str(USART1,"AT+CWJAP=\"TP-LINK_279486\",\"feliform\"\r\n");
//	 timer_sleep(1000);
//	 trace_printf("data: %s \r\n", rx_buffer);
//	 send_str(USART1,"AT+CIPMUX=1\r\n");
//	 timer_sleep(1000);
//	 trace_printf("data: %s \r\n", rx_buffer);
//	 send_str(USART1,"AT+CIPSERVER=1,3333\r\n");
	 while(1){
		 esp8266_cwlap(&esp);
		 //trace_printf("data111: 111\r\n");
		 //timer_sleep(1000);
//		 send_str(USART1,"AT\r\n");
		 //esp8266_gmr(&esp);
		 //timer_sleep(1000);
//		 int delay = 1;
//		 for(uint16_t j = 0; j <= register_get_max(); j++){
//			 for(uint8_t i = 0; i < register_get_led_count(); ++i){
//				 set_led_color(i,j,0,0);
//			 }
//			 timer_sleep(delay);
//		 }
//		 //trace_printf("data: %s \r\n", rx_buffer);
//		 for(uint16_t j = register_get_max()+1; j > 0; j--){
//			 for(uint8_t i = 0; i < register_get_led_count(); ++i){
//				 set_led_color(i,j-1,0,0);
//			 }
//			 timer_sleep(delay);
//		 }
//		 for(uint16_t j = 0; j <= register_get_max(); j++){
//			 for(uint8_t i = 0; i < register_get_led_count(); ++i){
//				 set_led_color(i,0,j,0);
//			 }
//			 timer_sleep(delay);
//		 }
//		 for(uint16_t j = register_get_max()+1; j > 0; j--){
//			 for(uint8_t i = 0; i < register_get_led_count(); ++i){
//				 set_led_color(i,0,j-1,0);
//			 }
//			 timer_sleep(delay);
//		 }
//		 for(uint16_t j = 0; j <= register_get_max(); j++){
//			 for(uint8_t i = 0; i < register_get_led_count(); ++i){
//				 set_led_color(i,0,0,j);
//			 }
//			 timer_sleep(delay);
//		 }
//		 for(uint16_t j = register_get_max()+1; j > 0; j--){
//			 for(uint8_t i = 0; i < register_get_led_count(); ++i){
//				 set_led_color(i,0,0,j-1);
//			 }
//			 timer_sleep(delay);
//		 }
	 }
}
#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
