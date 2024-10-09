#ifndef MENU_FUNCTIONS_H
#define MENU_FUNCTIONS_H

#include "at32f403a_407_board.h" // Добавьте, если необходимо
#include <stdbool.h>
#include "ssd1306_menu.h"
#include "encoder_navigation.h"

typedef enum {
    COLOR_INVERSION_OFF,
    COLOR_INVERSION_ON,
    COLOR_INVERSION_APPLY
} ColorInversionState_t;

extern volatile bool isEditing;
extern uint32_t functionStartTime; // Время запуска функции Battery
extern volatile ColorInversionState_t colorInversionState;
extern uint32_t last_battery_update;
extern bool isInversed;
extern volatile bool button_pressed;
extern volatile bool functionRunning;
extern uint16_t eeprom;
extern volatile bool isPlaying;
void Geoscan_logotype(uint8_t);
void Screen_off();
void input_number_handler(uint8_t);
void Brightness_selected(uint8_t);
void color_inversion(uint8_t);
void Information(void);
#endif /* MENU_FUNCTIONS_H */
