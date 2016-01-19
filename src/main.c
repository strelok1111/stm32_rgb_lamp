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
#include "effect/wave.h"
#include "effect/color_shift.h"
#include "effect/heart_beat.h"
#include "random.h"

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
WaveEffect wave_effect;
ColourShiftEffect color_shift;
HeartBeatEffect heart_beat;
typedef struct {
	void (*get_frame)(void *);
} Effect;
Effect *current_effect;

void TIM2_IRQHandler(void){
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET){
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		register_timer_tick();
	}
}
void TIM3_IRQHandler(void){
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET){
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		if(current_effect && current_effect->get_frame)
			current_effect->get_frame(current_effect);
	}
}
void USART1_IRQHandler(){
    if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET){
    	esp8266_recive_usart_byte(&esp,(uint8_t)(USART_ReceiveData(USART1)& 0xFF));
    }
}

void recived_data_from_esp(Esp8266 *esp,Esp8266Connect* connect){
	char *resp = "111!";
	esp8266_send_data(esp,connect,resp,strlen(resp));
}
void init_main_timer3(void){
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	TIM_TimeBaseInitTypeDef TIM_InitStructure;
	TIM_TimeBaseStructInit(&TIM_InitStructure);
	TIM_InitStructure.TIM_Prescaler = 36000 - 1;
	TIM_InitStructure.TIM_Period = 20;
	TIM_TimeBaseInit(TIM3, &TIM_InitStructure);
	TIM_Cmd(TIM3, ENABLE);

	NVIC_EnableIRQ(TIM3_IRQn);
	TIM_ITConfig(TIM3, TIM_DIER_UIE, ENABLE);
}

int main(int argc, char* argv[]){

	 registers_gpio_init();
	 register_init_timer2();
	 init_main_timer3();
	 timer_start();
	 random_init();

	 color_shift_effect_init(&color_shift);
	 ColourRgb col1 = colour_get_random_colour();
	 ColourRgb col2 = colour_get_random_colour();
	 ColourRgb col3 = colour_get_random_colour();
	 color_shift_add_color(&color_shift,&col1,random_number(1000));
	 color_shift_add_color(&color_shift,&col2,random_number(1000));
	 color_shift_add_color(&color_shift,&col3,random_number(1000));
	 current_effect = (Effect *)(&color_shift);
	 uint32_t random32bit = random_number(1000);
	 random32bit = random_number(1000);
	 random32bit = random_number(1000);
//	 heart_beat_effect_init(&heart_beat);
//	 current_effect = (Effect *)(&heart_beat);

//	 wave_effect_init(&wave_effect);
//
//	 Wave new_wave;
//	 wave_effecct_init_wave(&new_wave);
//	 new_wave.speed = 20;
//	 new_wave.spin = CW;
//	 new_wave.wave_colours[1].r = 20;
//	 new_wave.wave_colours[1].g = 50;
//	 new_wave.wave_colours[1].b = 10;
//
//	 new_wave.wave_colours[2].r = 50;
//	 new_wave.wave_colours[2].g = 170;
//	 new_wave.wave_colours[2].b = 80;
//
//	 new_wave.wave_colours[3].r = 20;
//	 new_wave.wave_colours[3].g = 50;
//	 new_wave.wave_colours[3].b = 10;
//
//
//
//	 Wave new_wave1;
//	 wave_effecct_init_wave(&new_wave1);
//	 new_wave1.speed = 10;
//	 new_wave1.spin = CW;
//	 new_wave1.wave_colours[1].r = 0;
//	 new_wave1.wave_colours[1].g = 0;
//	 new_wave1.wave_colours[1].b = 10;
//
//	 new_wave1.wave_colours[2].r = 200;
//	 new_wave1.wave_colours[2].g = 0;
//	 new_wave1.wave_colours[2].b = 80;
//
//	 new_wave1.wave_colours[3].r = 0;
//	 new_wave1.wave_colours[3].g = 0;
//	 new_wave1.wave_colours[3].b = 10;
//
//	 wave_effect_add_wave(&wave_effect,&new_wave1);
//	 wave_effect_add_wave(&wave_effect,&new_wave);
//	 current_effect = (Effect *)(&wave_effect);

//	 init_USART_wifi();
//	 esp8266_init(&esp);
//	 esp.at.send_raw_funct = send_data_raw_USART_wifi;
//	 esp.data_recived = recived_data_from_esp;
//	 esp8266_set_cwmode_station(&esp);
//	 esp8266_set_multiple_connections(&esp);
//	 esp8266_connect_to_wifi(&esp);
//	 esp8266_start_server(&esp);

	 while(1);
}
#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
