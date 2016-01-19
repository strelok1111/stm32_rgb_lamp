/*
 * color_shift.c
 *
 *  Created on: 28 сент. 2015 г.
 *      Author: strelok
 */
#include "color_shift.h"

void color_shift_effect_get_frame(ColourShiftEffect *color_shift){
	if(color_shift -> colours_count == 0){
		return;
	}
	if(color_shift -> colours_count == 1){
		set_all_led_color_rgb(&(color_shift -> shift_colours[0]));
		return;
	}
	uint8_t next_colour = (color_shift -> current_col + 1) % color_shift -> colours_count;
	uint32_t delay = color_shift -> delay[color_shift -> current_col];
	double shift = (1.0 / delay) * color_shift -> cur_delay_step_delay;
	ColourRgb col = colour_add_with_shift_to_rgb(&(color_shift -> shift_colours[color_shift -> current_col]),&(color_shift -> shift_colours[next_colour]),shift);
	set_all_led_color_rgb(&col);
	color_shift -> cur_delay_step_delay++;
	if(color_shift -> cur_delay_step_delay == delay){
		color_shift -> current_col = next_colour;
		color_shift -> cur_delay_step_delay = 0;
	}
}
void color_shift_effect_init(ColourShiftEffect *color_shift){
	color_shift -> get_frame = color_shift_effect_get_frame;
	color_shift -> colours_count = 0;
	color_shift -> current_col = 0;
	color_shift -> cur_delay_step_delay = 0;
}
void color_shift_add_color(ColourShiftEffect *color_shift,ColourRgb *color,uint32_t delay){
	if (color_shift -> colours_count == 0){
		color_shift -> shift_colours = malloc(sizeof(ColourRgb));
		color_shift -> delay =  malloc(sizeof(uint32_t));
	}else{
		color_shift -> shift_colours = (ColourRgb*) realloc(color_shift -> shift_colours, sizeof(ColourRgb) * (color_shift -> colours_count + 1));
		color_shift -> delay = (uint32_t *) realloc(color_shift -> delay,sizeof(uint32_t) * (color_shift -> colours_count + 1));
	}
	color_shift -> shift_colours[color_shift -> colours_count] = *color;
	color_shift -> delay[color_shift -> colours_count] = delay;
	color_shift -> colours_count++;
}

