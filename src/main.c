
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
#define CONNECT_STRING (__I uint8_t*) 0x8008000


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

volatile Esp8266 esp;
WaveEffect wave_effect;
ColourShiftEffect color_shift;
HeartBeatEffect heart_beat;
typedef struct {
	void (*get_frame)(void *);
} Effect;
volatile Effect *current_effect;

#define RESPONSE_BUFFER_LENGTH_R 1000
char response_buffer[RESPONSE_BUFFER_LENGTH_R];

#define STATUS_COMMAND_CHAR_ID 'S'

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
	uint32_t response_length = 0;
	memset(response_buffer,0,RESPONSE_BUFFER_LENGTH_R);
	uint8_t first_char = connect -> data_buffer[0];
	uint8_t second_char = connect -> data_buffer[1];
	if(first_char == WAVE_EFFECT_ID){
		if(current_effect != &wave_effect){
			current_effect = (Effect *)(&wave_effect);
		}
		strtok(connect -> data_buffer,",");
		if(second_char == WAVE_COMMAND_ADD_WAVE_ID){
			Wave new_wave;
			wave_effect_init_wave(&new_wave);
			for(uint8_t cur_led = 0;cur_led < LED_COUNT;cur_led++){
				new_wave.wave_colours[cur_led].r = atoi(strtok(NULL,","));
				new_wave.wave_colours[cur_led].g = atoi(strtok(NULL,","));
				new_wave.wave_colours[cur_led].b = atoi(strtok(NULL,","));
			}
			new_wave.speed = atoi(strtok(NULL,","));
			if(atoi(strtok(NULL,","))){
				new_wave.spin = CW;
			}else{
				new_wave.spin = CCW;
			};
			wave_effect_add_wave(&wave_effect,&new_wave);
		}
		if(second_char == WAVE_COMMAND_REMOVE_WAVE_ID){
			wave_effect_remove_wave(&wave_effect,atoi(strtok(NULL,",")));
		}
		if(second_char == WAVE_COMMAND_CLEAR_ID){
			wave_effect_clear(&wave_effect);
		}
		strcat(response_buffer,"OK");
	}else if(first_char == HEART_BEAT_EFFECT_ID){
		if(current_effect != &heart_beat){
			current_effect = (Effect *)(&heart_beat);
		}
		strtok(connect -> data_buffer,",");
		if(second_char == HEART_BEAT_COMMAND_SET_COLOR_ID){
			uint8_t r = atoi(strtok(NULL,","));
			uint8_t g = atoi(strtok(NULL,","));
			uint8_t b = atoi(strtok(NULL,","));
			heart_beat_effect_set_color(&heart_beat,r,g,b);
		}
		if(second_char == HEART_BEAT_COMMAND_SET_SPEED_ID){
			heart_beat_effect_set_bpm(&heart_beat,atoi(strtok(NULL,",")));
		}
		strcat(response_buffer,"OK");
	}else if(first_char == COLOR_SHIFT_EFFECT_ID){
		if(current_effect != &color_shift){
			current_effect = (Effect *)(&color_shift);
		}
		strtok(connect -> data_buffer,",");
		if(second_char == COLOR_SHIFT_COMMAND_ADD_COLOR_ID){
			ColourRgb col;
			col.r = atoi(strtok(NULL,","));
			col.g = atoi(strtok(NULL,","));
			col.b = atoi(strtok(NULL,","));
			uint32_t delay = atoi(strtok(NULL,","));
			color_shift_add_color(&color_shift,&col,delay);
		}
		if(second_char == COLOR_SHIFT_COMMAND_REMOVE_COLOR_ID){
			color_shift_remove_color(&color_shift,atoi(strtok(NULL,",")));
		}
		if(second_char == COLOR_SHIFT_COMMAND_MODIFY_COLOR_ID){
			char *index = strtok(NULL,",");
			uint8_t ind = atoi(index);
			color_shift.shift_colours[ind].r = atoi(strtok(NULL,","));
			color_shift.shift_colours[ind].g = atoi(strtok(NULL,","));
			color_shift.shift_colours[ind].b = atoi(strtok(NULL,","));
			color_shift.delay[ind] = atoi(strtok(NULL,","));
		}
		if(second_char == COLOR_SHIFT_COMMAND_CLEAR_ID){
			color_shift_clear(&color_shift);
		}
		strcat(response_buffer,"OK");
	}else if(first_char == STATUS_COMMAND_CHAR_ID){
		if(current_effect == &color_shift){
			strcat(response_buffer,"C");
			for (uint8_t i = 0; i < color_shift.colours_count; i++) {
				colour_append_to_str(&(color_shift.shift_colours[i]),response_buffer);
				char delay_buffer[11];
				itoa(color_shift.delay[i],delay_buffer,10);
				strcat(response_buffer,",");
				strcat(response_buffer,delay_buffer);
				if(i != color_shift.colours_count - 1){
					strcat(response_buffer,",");
				}
			}
		}
		if(current_effect == &heart_beat){
			strcat(response_buffer,"H");
			colour_append_to_str(&(heart_beat.colour),response_buffer);
			char speed_buffer[3];
			itoa(heart_beat.speed_bpm,speed_buffer,10);
			strcat(response_buffer,",");
			strcat(response_buffer,speed_buffer);
		}
		if(current_effect == &wave_effect){
			strcat(response_buffer,"W");
			for (uint8_t i = 0; i < wave_effect.waves_count; i++) {
				for (uint8_t j = 0; j < LED_COUNT; j++) {
					colour_append_to_str(&(wave_effect.waves[i].wave_colours[j]),response_buffer);
					if(j != LED_COUNT - 1){
						strcat(response_buffer,",");
					}
				}
				char speed_buffer[4];
				itoa(wave_effect.waves[i].speed,speed_buffer,10);
				strcat(response_buffer,",");
				strcat(response_buffer,speed_buffer);

				strcat(response_buffer,",");
				if(wave_effect.waves[i].spin == CCW){
					strcat(response_buffer,"CCW");
				}else{
					strcat(response_buffer,"CW");
				}
				if(i != wave_effect.waves_count - 1){
					strcat(response_buffer,",");
				}
			}
		}

	}
	strcat(response_buffer,";");
	response_length = strlen(response_buffer);
	esp8266_send_data(esp,connect,response_buffer,response_length);
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
	NVIC_SetPriority(TIM3_IRQn,10);
	TIM_ITConfig(TIM3, TIM_DIER_UIE, ENABLE);
}
void set_start_serve_callback(Esp8266 *esp){
	esp8266_send_command_with_callback(esp,"AT+CIPSERVER=1,3355\r\n",NULL);
}
void set_set_ip_callback(Esp8266 *esp){
	esp8266_send_command_with_callback(esp,"AT+CIPSTA=\"172.16.1.105\"\r\n",set_start_serve_callback);
}
void set_connect_to_wifi_callback(Esp8266 *esp){
	esp8266_send_command_with_callback(esp,CONNECT_STRING,set_set_ip_callback);
}
void set_multiple_connections_callback(Esp8266 *esp){
	esp8266_send_command_with_callback(esp,"AT+CIPMUX=1\r\n",set_connect_to_wifi_callback);
}
void reset_esp(Esp8266 *esp){
	esp8266_send_command_with_callback(esp,"AT+CWMODE=1\r\n",set_multiple_connections_callback);
}
void esp_init(void){
	init_USART_wifi();
	esp.at.send_raw_funct = send_data_raw_USART_wifi;
	esp.data_recived = recived_data_from_esp;
	esp.on_reset = reset_esp;
	esp8266_init(&esp);
}



int main(int argc, char* argv[]){
	 registers_gpio_init();
	 register_init_timer2();
	 init_main_timer3();
	 esp_init();
	 color_shift_effect_init(&color_shift);
	 heart_beat_effect_init(&heart_beat);
	 wave_effect_init(&wave_effect);
	 //esp8266_send_command(&esp,"AT+CIOBAUD=115200\r\n");
	 while(1);
}
#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
