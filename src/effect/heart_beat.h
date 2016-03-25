/*
 * heart_beat.h
 *
 *  Created on: 26 сент. 2015 г.
 *      Author: strelok
 */

#ifndef HEART_BEAT_H_
#define HEART_BEAT_H_

#include "../colour/colour.h"
#include "color_shift.h"

typedef struct {
	void (*get_frame)(void *);
	ColourRgb colour;
	uint8_t speed_bpm;
	ColourShiftEffect color_shift;
} HeartBeatEffect;
#define HEART_BEAT_EFFECT_ID 'H'
#define HEART_BEAT_COMMAND_SET_COLOR_ID 'C'
#define HEART_BEAT_COMMAND_SET_SPEED_ID 'S'

extern void heart_beat_effect_init(HeartBeatEffect *);
extern void heart_beat_effect_set_color(HeartBeatEffect *,uint8_t,uint8_t,uint8_t);
extern void heart_beat_effect_set_bpm(HeartBeatEffect *,uint8_t);
#endif /* HEART_BEAT_H_ */
