#include "../colour/colour.h"
#include "../led_register/led_register.h"
#ifndef _WAVE
#define _WAVE
typedef enum {CW,CCW} wave_spin;
typedef struct {
	wave_spin spin;
	uint16_t speed;
	uint16_t current_step;
	uint8_t current_start;
	ColourRgb wave_colours[LED_COUNT];
	float steps_red[LED_COUNT];
	float steps_green[LED_COUNT];
	float steps_blue[LED_COUNT];
} Wave;
typedef struct {
	void (*get_frame)(void *);
	Wave *waves;
	uint8_t waves_count;
} WaveEffect;
extern void wave_effect_init(WaveEffect *);
extern void wave_effect_add_wave(WaveEffect *,Wave *);
extern void wave_effecct_init_wave(Wave*);
#endif
