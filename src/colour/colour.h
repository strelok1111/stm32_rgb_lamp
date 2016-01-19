
#ifndef _COLOR
#define _COLOR


#include <stdint-gcc.h>
#include <stdlib.h>
typedef struct  {
	uint8_t r;
	uint8_t g;
	uint8_t b;
} ColourRgb;

typedef struct  {
	int16_t h;
	float s;
	float v;
} ColourHsv;

extern void set_all_led_color_rgb(ColourRgb *);
ColourRgb colour_add_with_shift_to_rgb(ColourRgb *,ColourRgb *,double);
extern ColourRgb colour_get_random_colour();


#endif
