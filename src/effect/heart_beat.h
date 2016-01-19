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

extern void heart_beat_effect_init(HeartBeatEffect *);
#endif /* HEART_BEAT_H_ */
