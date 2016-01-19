#include "heart_beat.h"
#include "color_shift.h"

void heart_beat_effect_get_frame(HeartBeatEffect *heart_beat_effect){
	color_shift_effect_get_frame(&(heart_beat_effect -> color_shift));
}
void heart_beat_effect_init(HeartBeatEffect *heart_beat_effect){
	color_shift_effect_init(&(heart_beat_effect -> color_shift));
	(heart_beat_effect -> colour).r = 255;
	(heart_beat_effect -> colour).g = 0;
	(heart_beat_effect -> colour).b = 0;

	ColourRgb col_b;
	col_b.r = 0;
	col_b.g = 0;
	col_b.b = 0;

	color_shift_add_color(&(heart_beat_effect -> color_shift),&col_b,15);
	color_shift_add_color(&(heart_beat_effect -> color_shift),&(heart_beat_effect -> colour),15);
	color_shift_add_color(&(heart_beat_effect -> color_shift),&col_b,15);
	color_shift_add_color(&(heart_beat_effect -> color_shift),&(heart_beat_effect -> colour),50);
	heart_beat_effect -> get_frame = heart_beat_effect_get_frame;
}
