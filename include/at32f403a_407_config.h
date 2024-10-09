#ifndef ENCODER_H
#define ENCODER_H

#include "at32f403a_407_board.h" // Добавьте, если необходимо

void wk_tmr2_init(void);
void wk_nvic_config(void);
void wk_exint_config(void);
void wk_i2c1_init(void);
void wk_periph_clock_config(void);
void wk_system_clock_config(void);

#endif /* ENCODER_H */
