/*
 * color_shift.h
 *
 *  Created on: 28 сент. 2015 г.
 *      Author: strelok
 */
#include "../colour/colour.h"
#include "../led_register/led_register.h"

#ifndef COLOR_SHIFT_H_
#define COLOR_SHIFT_H_

#define COLOR_SHIFT_EFFECT_ID 'C'
#define COLOR_SHIFT_COMMAND_ADD_COLOR_ID 'A'
#define COLOR_SHIFT_COMMAND_REMOVE_COLOR_ID 'R'
#define COLOR_SHIFT_COMMAND_MODIFY_COLOR_ID 'M'
#define COLOR_SHIFT_COMMAND_CLEAR_ID 'C'
typedef struct {
	void (*get_frame)(void *);
	ColourRgb *shift_colours;
	uint8_t colours_count;
	uint8_t current_col;
	uint32_t *delay;
	uint32_t cur_delay_step_delay;

} ColourShiftEffect;
extern void color_shift_add_color(ColourShiftEffect *,ColourRgb *,uint32_t );
extern void color_shift_effect_init(ColourShiftEffect *);
extern void color_shift_remove_color(ColourShiftEffect *,uint8_t index);
extern void color_shift_clear(ColourShiftEffect *);

#endif /* COLOR_SHIFT_H_ */
