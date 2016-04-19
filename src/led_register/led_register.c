#include <stm32f10x_gpio.h>
#include "led_register.h"

volatile uint8_t color_data[CHA_COUNT];
volatile uint8_t global_counter = 0;

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
void set_led_color_rgb(uint8_t led_index,ColourRgb* color){
	set_led_color(led_index,color -> r,color -> g,color -> b);
}
void set_all_led_color_rgb(ColourRgb* color){
	for(uint8_t i = 0;i < LED_COUNT;i++ ){
		set_led_color_rgb(i,color);
	}
}
void set_led_color_hsv(uint8_t led_index,ColourHsv* color){
	ColourRgb col_rgb;
	colour_set_rgb_from_hsv(&col_rgb,&color);
	set_led_color_rgb(led_index,&col_rgb);
}

void set_led_array(char* data){
	memccpy(color_data,data,0,CHA_COUNT);
}

void register_timer_tick(void){
	for(uint8_t i = 0; i < 16; i++){
		write_data_channel_1(color_data[i] > global_counter);
		write_data_channel_2(color_data[i + 16] > global_counter);
		write_data_channel_3(color_data[i + 32] > global_counter);
		touch_register_clock();
	}
	touch_register_latch();
	++global_counter;
}

void register_init_timer2(void){
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	TIM_TimeBaseInitTypeDef TIM_InitStructure;
	TIM_TimeBaseStructInit(&TIM_InitStructure);
	TIM_InitStructure.TIM_Prescaler = TIMER_PRESCALER;
	TIM_InitStructure.TIM_Period = 1;
	TIM_TimeBaseInit(TIM2, &TIM_InitStructure);
	TIM_Cmd(TIM2, ENABLE);

	NVIC_EnableIRQ(TIM2_IRQn);
	NVIC_SetPriority(TIM2_IRQn,10);
	TIM_ITConfig(TIM2, TIM_DIER_UIE, ENABLE);
}

uint8_t register_get_led_count(void){
	return LED_COUNT;
}
uint8_t register_get_max(void){
	return MAX_COUNTER;
}
