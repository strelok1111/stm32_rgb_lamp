#define REGISTERS_COUNT 2
#define CHA_PER_REGISTER 8
#define CHA_COUNT 48
#define MAX_COUNTER 255
#define LED_COUNT 16

#define TIMER_PRESCALER 2600

extern void registers_gpio_init(void);
extern void set_led_color(uint8_t,uint8_t,uint8_t,uint8_t);
extern void register_timer_tick(void);
extern void register_init_timer2(void);
extern uint8_t register_get_led_count(void);
extern uint8_t register_get_max(void);
