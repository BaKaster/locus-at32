#include "at32f403a_407.h"
#include "at32f403a_407_board.h"
#include "at32f403a_407_int.h"
#include "encoder_navigation.h"


#define DELAY                            100
#define FAST                             1
#define SLOW                             4
#define BUTTON_1_PORT					 GPIOA
#define INIT_BUTTON_PARAM {0, 0, 0}



void button_isr(void);
void led_cycle();
void check_button_mn(uint8_t button);
void wk_tmr7_init(void);
void button_exint_init(void);

typedef struct {
	uint8_t status; // 0 - не зажата, 1 - зажата
	uint64_t start_click;
	uint8_t but_number;
}button_param;
