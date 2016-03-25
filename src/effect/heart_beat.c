#include "heart_beat.h"
#include "color_shift.h"

void heart_beat_effect_get_frame(HeartBeatEffect *heart_beat_effect){
	color_shift_effect_get_frame(&(heart_beat_effect -> color_shift));
}
void heart_beat_effect_init(HeartBeatEffect *heart_beat_effect){
	color_shift_effect_init(&(heart_beat_effect -> color_shift));
	heart_beat_effect -> speed_bpm = 60;
	(heart_beat_effect -> colour).r = 255;
	(heart_beat_effect -> colour).g = 0;
	(heart_beat_effect -> colour).b = 0;

	ColourRgb col_b;
	col_b.r = 0;
	col_b.g = 0;
	col_b.b = 0;

	color_shift_add_color(&(heart_beat_effect -> color_shift),&col_b,(60.0 / heart_beat_effect -> speed_bpm) * 15);
	color_shift_add_color(&(heart_beat_effect -> color_shift),&(heart_beat_effect -> colour),(60.0 / heart_beat_effect -> speed_bpm) * 15);
	color_shift_add_color(&(heart_beat_effect -> color_shift),&col_b,(60.0 / heart_beat_effect -> speed_bpm) * 15);
	color_shift_add_color(&(heart_beat_effect -> color_shift),&(heart_beat_effect -> colour),(60.0 / heart_beat_effect -> speed_bpm) * 50);
	heart_beat_effect -> get_frame = heart_beat_effect_get_frame;
}
void heart_beat_effect_set_color(HeartBeatEffect *heart_beat_effect,uint8_t red,uint8_t green,uint8_t blue){
	(heart_beat_effect -> colour).r = red;
	(heart_beat_effect -> colour).g = green;
	(heart_beat_effect -> colour).b = blue;
	heart_beat_effect -> color_shift.shift_colours[1] = heart_beat_effect -> colour;
	heart_beat_effect -> color_shift.shift_colours[3] = heart_beat_effect -> colour;

}
void heart_beat_effect_set_bpm(HeartBeatEffect *heart_beat_effect,uint8_t bpm){
	heart_beat_effect -> speed_bpm = bpm;
	heart_beat_effect -> color_shift.delay[0] = (60.0 / heart_beat_effect -> speed_bpm) * 15;
	heart_beat_effect -> color_shift.delay[1] = (60.0 / heart_beat_effect -> speed_bpm) * 15;
	heart_beat_effect -> color_shift.delay[2] = (60.0 / heart_beat_effect -> speed_bpm) * 15;
	heart_beat_effect -> color_shift.delay[3] = (60.0 / heart_beat_effect -> speed_bpm) * 50;
}
