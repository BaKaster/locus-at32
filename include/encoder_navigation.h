#ifndef ENCODER_NAVIGATION
#define ENCODER_NAVIGATION

#include "at32f403a_407_board.h" // Добавьте, если необходимо
#include <stdbool.h>
#include "ssd1306_menu.h"

extern const uint32_t BUTTON_DEBOUNCE_DELAY;
void encoder_handler(bool);
void TMR2_GLOBAL_IRQHandler(void);
extern int32_t temp_encoder;
extern uint32_t buttonStarttime;
extern bool stopBattery;
extern volatile uint32_t button_press_time;
extern int32_t encoder_delta;
void EXINT1_IRQHandler(void);
void button_handlers();

#endif /* ENCODER_NAVIGATION */
