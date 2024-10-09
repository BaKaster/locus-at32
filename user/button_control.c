#include"button_control.h"


uint8_t g_speed = FAST;
button_param button_1 = INIT_BUTTON_PARAM;
button_param button_2 = INIT_BUTTON_PARAM;

void button_isr(void) {
	/* delay 5ms */
	delay_ms(5);

	/* clear interrupt pending bit */
	exint_flag_clear(EXINT_LINE_15);

	/* check input pin state */
	if (SET == gpio_input_data_bit_read(BUTTON_1_PORT, USER_BUTTON_PIN)) {
		if (g_speed == SLOW)
			g_speed = FAST;
		else
			g_speed = SLOW;
	}
}
void init_button_par(){
	button_1.but_number = 1;
	button_2.but_number = 2;
}

void led_cycle() {
	at32_led_toggle(LED2);
	delay_ms(g_speed * DELAY);
	at32_led_toggle(LED3);
	delay_ms(g_speed * DELAY);
	at32_led_toggle(LED4);
	delay_ms(g_speed * DELAY);
}

void check_button_mn(uint8_t button) {
	if (button == 1) {
		check_button(&button_1);
	} else if (button == 2) {
		check_button(&button_2);
	}
}


void check_button(button_param *button) {
	if (!button->status) {
		button->status = 1;
		button->start_click = get_time();
	} else {
		int u = get_time();
		uint16_t i = get_time() - button->start_click;
	if(get_time() - button->start_click > 100 && button->but_number == 2) {
		button->status = 0;
	} else if(get_time() - button->start_click > 15){
		if(selected_item > 0) {
			selected_item++;
		} else {
			selected_item = 0;
		}
		display_menu(current_menu, selected_item);
		button->status = 0;
	}else {

		button->status = 0;
	}

}
}

void wk_tmr7_init(void) {
/* add user code begin tmr7_init 0 */

/* add user code end tmr7_init 0 */

/* add user code begin tmr7_init 1 */

/* configure counter settings */
tmr_base_init(TMR7, 65535, 200);
tmr_cnt_dir_set(TMR7, TMR_COUNT_UP);
tmr_period_buffer_enable(TMR7, FALSE);

/* configure primary mode settings */
tmr_primary_mode_select(TMR7, TMR_PRIMARY_SEL_OVERFLOW);

tmr_counter_enable(TMR7, TRUE);

tmr_interrupt_enable(TMR7, TMR_OVF_INT, TRUE);
}

void button_exint_init(void) {
exint_init_type exint_init_struct;


gpio_exint_line_config(GPIO_PORT_SOURCE_GPIOA, GPIO_PINS_SOURCE3);
gpio_exint_line_config(GPIO_PORT_SOURCE_GPIOA, GPIO_PINS_SOURCE4);

exint_default_para_init(&exint_init_struct);
exint_init_struct.line_enable = TRUE;
exint_init_struct.line_mode = EXINT_LINE_INTERRUPUT;
exint_init_struct.line_select = EXINT_LINE_3 | EXINT_LINE_4;
exint_init_struct.line_polarity = EXINT_TRIGGER_BOTH_EDGE;
exint_init(&exint_init_struct);

nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);

}
