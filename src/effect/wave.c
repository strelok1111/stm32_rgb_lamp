#include "wave.h"
void wave_effect_recalc_steps(Wave *wave){
	wave-> steps_red[LED_COUNT - 1] = (wave -> wave_colours[LED_COUNT - 1].r - wave -> wave_colours[0].r) / (float)(wave -> speed);
	for(uint8_t i = 0;i < LED_COUNT - 1; i ++){
		wave-> steps_red[i] = (wave -> wave_colours[i].r - wave -> wave_colours[i + 1].r) /  (float)(wave -> speed);
	}
	wave-> steps_blue[LED_COUNT - 1] = (wave -> wave_colours[LED_COUNT - 1].b - wave -> wave_colours[0].b) / (float)(wave -> speed);
	for(uint8_t i = 0;i < LED_COUNT - 1; i ++){
		wave-> steps_blue[i] = (wave -> wave_colours[i].b - wave -> wave_colours[i + 1].b) /  (float)(wave -> speed);
	}
	wave-> steps_green[LED_COUNT - 1] = (wave -> wave_colours[LED_COUNT - 1].g - wave -> wave_colours[0].g) / (float)(wave -> speed);
	for(uint8_t i = 0;i < LED_COUNT - 1; i ++){
		wave-> steps_green[i] = (wave -> wave_colours[i].g - wave -> wave_colours[i + 1].g) /  (float)(wave -> speed);
	}
}
void wave_effect_update_steps(WaveEffect *wave_effect){
	for(uint8_t i = 0;i < wave_effect -> waves_count;i++){
		Wave *wave = &(wave_effect -> waves[i]);
		wave -> current_step++;
		if(wave -> current_step > wave -> speed){
			wave -> current_step = 0;
			if(wave -> current_start == 0)
				wave -> current_start = LED_COUNT - 1;
			else
				wave -> current_start--;
		}
	}
}
void wave_effect_get_frame(WaveEffect *wave_effect){
	for(uint8_t cur_led = 0;cur_led < LED_COUNT;cur_led++){
		ColourRgb result_colour = {0,0,0};
		for(uint8_t i = 0;i < wave_effect -> waves_count;i++){
			ColourRgb cur_wave_colour;
			Wave *wave = &(wave_effect -> waves[i]);
			uint8_t index;
			if(wave -> spin == CW)
				index = (wave -> current_start + cur_led) % LED_COUNT;
			else if (wave -> spin == CCW)
				index = (wave -> current_start + LED_COUNT - 1 - cur_led) % LED_COUNT;
			uint8_t step_index = index == 0 ? LED_COUNT - 1 : index - 1;
			cur_wave_colour.r = (uint8_t)(wave -> wave_colours[index].r + wave-> steps_red[step_index] * wave -> current_step);
			cur_wave_colour.g = (uint8_t)(wave -> wave_colours[index].g + wave-> steps_green[step_index] * wave -> current_step);
			cur_wave_colour.b = (uint8_t)(wave -> wave_colours[index].b + wave-> steps_blue[step_index] * wave -> current_step);
			colour_add_to_rgb(&result_colour,&cur_wave_colour);
		}
		set_led_color_rgb(cur_led,&result_colour);
	}
	wave_effect_update_steps(wave_effect);
}
void wave_recalc_steps_all(WaveEffect *wave_effect){
	for (uint8_t i = 0; i < wave_effect -> waves_count; i++)
		wave_effect_recalc_steps(&(wave_effect -> waves[i]));
}
void wave_effect_init(WaveEffect *wave_effect){
	wave_effect -> get_frame = wave_effect_get_frame;
	wave_effect -> waves_count = 0;
}
void wave_effect_add_wave(WaveEffect *wave_effect,Wave *wave_to_add){
	if(wave_effect -> waves_count == 0){
		wave_effect -> waves = malloc(sizeof(Wave));
	}else{
		wave_effect -> waves = (Wave*) realloc(wave_effect -> waves,sizeof(Wave) * (wave_effect -> waves_count + 1));
	}
	if(wave_to_add -> speed == 0)
		wave_to_add -> speed = 1;
	wave_effect -> waves[wave_effect -> waves_count] = *wave_to_add;
	wave_effect -> waves_count++;
    wave_recalc_steps_all(wave_effect);
}
void wave_effect_remove_wave(WaveEffect *wave_effect,uint8_t index){
	if(wave_effect -> waves_count == 0 || index >= wave_effect -> waves_count  )
		return;
	else{
		for (uint8_t i = index + 1; i < wave_effect -> waves_count; i++) {
			wave_effect -> waves[i - 1] = wave_effect -> waves[i];
		}
		wave_effect -> waves_count--;
		wave_effect -> waves = (Wave*) realloc(wave_effect -> waves,sizeof(Wave) * (wave_effect -> waves_count));
		wave_recalc_steps_all(wave_effect);
	}

}
void wave_effect_init_wave(Wave *wave){
	 wave -> speed = 10;
	 wave -> spin = CCW;
	 wave -> current_start = 0;
	 wave -> current_step = 0;
	 for(int i = 0;i< LED_COUNT;i++){
		 wave -> wave_colours[i].r = 0;
		 wave -> wave_colours[i].g = 0;
		 wave -> wave_colours[i].b = 0;
		 wave -> steps_red[i] = 0.0;
		 wave -> steps_blue[i] = 0.0;
		 wave -> steps_green[i] = 0.0;
	 }
}
void wave_effect_clear(WaveEffect *wave_effect) {
	if (wave_effect -> waves_count == 0){
		return;
	}else{
		wave_effect -> waves_count = 0;
		free(wave_effect -> waves);
	}
}

