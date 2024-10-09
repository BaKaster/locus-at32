#include "at32f403a_407_clock.h"
#include "at32f403a_407_board.h"
#include "at32_video_ev_lcd.h"
#include "at32_video_ev_spi.h"
#include "images.h"
#include "encoder_navigation.h"
#include "ssd1306_menu.h"
#include "at32f403a_407_config.h"
#include "stdlib.h"
#include "stdio.h"
#include <wchar.h>
#include "menu_functions.h"
#include "eeprom.h"

#define MENU_ITEM_TYPE_FOLDER 0
#define MENU_ITEM_TYPE_ACTION 1
#define MAX_ITEM_LENGTH 20
volatile uint32_t current_time = 0;

#define DEBOUNCE_DELAY 50 // Задержка антидребезга в миллисекундах
#define LONG_PRESS_DELAY 1000 // Задержка для определения долгого нажатия

// Состояния конечного автомата
typedef enum {
	BUTTON_RELEASED, BUTTON_DEBOUNCING, BUTTON_PRESSED, BUTTON_LONG_PRESSED
} ButtonState;

ButtonState buttonState = BUTTON_RELEASED;
uint32_t buttonPressTime = 0;

char eeprom_val[5], run[2];
uint16_t eeprom;
volatile uint32_t button_press_time = 0;
#define    EXTEND_SRAM          0x00

void crm_configuration(void);
void nvic_configuration(void);

/**
 * @brief  initializ lcd display information
 * @param  none
 * @retval none
 */

void lcd_display_initial_info(void) {
//	  wchar_t time_str[10];
//	  swprintf(time_str, sizeof(time_str) / sizeof(wchar_t), L"%lu", current_time);
	lcd_clear(BACKGROUND);
	lcd_draw_filled_round_rect(2, 23, 126, 135, 7, BUTTONS);
	lcd_draw_filled_round_rect(5, 33, 120, 18, 7, BRIGHTED_SELECT);
	lcd_show_string(7, 60, 120, 100, &Montserrat_12, L"1 2 3 4 5 6 7 8 9 0",
	WHITE);
	lcd_show_string(7, 80, 120, 100, &Montserrat_12, L"й ц у к е н г ш щ з х",
	WHITE);
	lcd_show_string(7, 100, 120, 100, &Montserrat_12, L"ф ы в а п р о л д ж э",
	WHITE);
	lcd_show_string(7, 120, 20, 100, &Montserrat_12, L"я ч с м и т ь б ю ё",
	WHITE);
	lcd_show_string(7, 140, 120, 100, &Montserrat_12, L"?123 , яз пробел . ст",
	WHITE);

	st7735s_DrawBitmap(&Geoscan_logo, 3, 5);
//    draw_transparent_png(&Display, 4, 26);
//    lcd_draw_round_rect(2, 25, 121, 42, 7, WHITE);
//	lcd_draw_filled_round_rect(6, 50, 112, 12, 5, GEOSCAN_COLOR);
	lcd_display_buffer();
}

void extend_sram(void) {
	/* check if ram has been set to expectant size, if not, change eopb0 to 64k sram */
	if (((USD->eopb0) & 0xFF) != EXTEND_SRAM) {
		flash_unlock();
		/* erase user system data bytes */
		flash_user_system_data_erase();

		/* change sram size */
		flash_user_system_data_program((uint32_t) &USD->eopb0, EXTEND_SRAM);

		/* system reset */
		nvic_system_reset();
	}
}

/**
 * @brief  main function.
 * @param  none
 * @retval none
 */

int main(void) {
	system_clock_config();
	wk_system_clock_config();

	at32_board_init();

	crm_configuration();
	wk_exint_config();
	extend_sram();
	nvic_configuration();

	button_exint_init();

	wk_tmr7_init();

	init_button_par();

	delay_init();

//  uart_print_init(115200);
//  printf("usart init ok\r\n");

	flash_ee_init();
	wk_tmr2_init();
	lcd_init();

	tmr_counter_value_set(TMR2, 0);

	init_millis();
	menu_init();

	current_menu = MainMenu;
	selected_item = 0;
	apply_changes();
	display_menu(current_menu, selected_item);

	while (1) {
		current_time = get_millis();
		//delay_unblocked(100000);
		// lcd_display_initial_info();
		encoder_handler(true);
		//  lcd_display_initial_info();
		handleButton();

	}
}

void handleButton() {
	uint32_t currentTime = get_millis();
	bool buttonPressed = (gpio_input_data_bit_read(GPIOC, GPIO_PINS_2) == RESET);

	switch (buttonState) {
	case BUTTON_RELEASED:
		if (buttonPressed) {
			buttonState = BUTTON_DEBOUNCING;
			buttonPressTime = currentTime;
		}
		break;

	case BUTTON_DEBOUNCING:
		if (currentTime - buttonPressTime >= DEBOUNCE_DELAY) {
			if (buttonPressed) {
				buttonState = BUTTON_PRESSED;
				// Обработка короткого нажатия
				at32_led_toggle(LED2);
				button_handlers();
			} else {
				buttonState = BUTTON_RELEASED;
			}
		}
		break;

	case BUTTON_PRESSED:
		if (!buttonPressed) {
			buttonState = BUTTON_RELEASED;
		} else if (currentTime - buttonPressTime >= LONG_PRESS_DELAY) {
			buttonState = BUTTON_LONG_PRESSED;
			// Обработка долгого нажатия
		}
		break;

	case BUTTON_LONG_PRESSED:
		if (!buttonPressed) {
			buttonState = BUTTON_RELEASED;
		}
		break;
	}
}

/**
 * @brief  crm configuration
 * @param  none
 * @retval none
 */
void crm_configuration(void) {
	crm_periph_clock_enable(LCD_SPI_MASTER_DMA_CLK, TRUE);
	crm_periph_clock_enable(LCD_SPI_SELECTED_CLK, TRUE);
	crm_periph_clock_enable(CRM_IOMUX_PERIPH_CLOCK, TRUE);
	crm_periph_clock_enable(LCD_PEN_GPIO_CLK, TRUE);
	crm_periph_clock_enable(LCD_RST_GPIO_CLK, TRUE);
	crm_periph_clock_enable(LCD_BLK_GPIO_CLK, TRUE);
	crm_periph_clock_enable(LCD_DC_GPIO_CLK, TRUE);
	crm_periph_clock_enable(LCD_CS1_GPIO_CLK, TRUE);
	crm_periph_clock_enable(LCD_CS2_GPIO_CLK, TRUE);
	crm_periph_clock_enable(LCD_SPI_SCK_GPIO_CLK, TRUE);
	crm_periph_clock_enable(LCD_SPI_MOSI_GPIO_CLK, TRUE);
	crm_periph_clock_enable(LCD_SPI_MISO_GPIO_CLK, TRUE);
	crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
	crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK, TRUE);
	crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
	crm_periph_clock_enable(CRM_GPIOD_PERIPH_CLOCK, TRUE);
	crm_periph_clock_enable(CRM_IOMUX_PERIPH_CLOCK, TRUE);
	crm_periph_clock_enable(CRM_TMR2_PERIPH_CLOCK, TRUE);
	crm_periph_clock_enable(CRM_TMR3_PERIPH_CLOCK, TRUE);
	crm_periph_clock_enable(CRM_TMR6_PERIPH_CLOCK, TRUE);
	crm_periph_clock_enable(CRM_TMR7_PERIPH_CLOCK, TRUE);

//  dma_init_type dma_init_struct;
//  crm_periph_clock_enable(CRM_DMA1_PERIPH_CLOCK, TRUE);
//  dma_reset(LCD_SPI_MASTER_Tx_DMA_Channel);
//  dma_init_struct.buffer_size = 0xFFFE;
//  dma_init_struct.direction = DMA_DIR_MEMORY_TO_PERIPHERAL;
//  dma_init_struct.memory_base_addr = (uint32_t)LCD_SPI_MASTER_DR_Base;
//  dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_BYTE;
//  dma_init_struct.memory_inc_enable = TRUE;
//  dma_init_struct.peripheral_base_addr = (uint32_t)LCD_SPI_MASTER_DR_Base;
//  dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_BYTE;
//  dma_init_struct.peripheral_inc_enable = FALSE;
//  dma_init_struct.priority = DMA_PRIORITY_HIGH;
//  dma_init_struct.loop_mode_enable = FALSE;
//  dma_init(LCD_SPI_MASTER_Tx_DMA_Channel, &dma_init_struct);

//  nvic_irq_enable(LCD_SPI_MASTER_Tx_DMA_IRQn, 1, 0);
//  dma_interrupt_enable(LCD_SPI_MASTER_Tx_DMA_Channel,DMA_FDT_INT,TRUE);
}

/**
 * @brief  nvic configuration
 * @param  none
 * @retval none
 */
void nvic_configuration(void) {
	/* 2 bit for pre-emption priority,2 bits for subpriority */
	nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);

	/* configure dma1 channel3 for the spi transmit */
	nvic_irq_enable(LCD_SPI_MASTER_Tx_DMA_IRQn, 0, 2);
	nvic_irq_enable(SysTick_IRQn, 0, 0);
	nvic_irq_enable(EXINT2_IRQn, 1, 0);
	nvic_irq_enable(TMR2_GLOBAL_IRQn, 2, 0);
	nvic_irq_enable(EXINT4_IRQn, 1, 0);
	nvic_irq_enable(EXINT3_IRQn, 1, 0);
	nvic_irq_enable(EXINT15_10_IRQn, 1, 0);
	tmr_interrupt_enable(TMR2, TMR_C2_INT, TRUE);
	nvic_irq_enable(TMR3_GLOBAL_IRQn, 1, 0);
	nvic_irq_enable(TMR6_GLOBAL_IRQn, 1, 0);
	nvic_irq_enable(TMR7_GLOBAL_IRQn, 1, 0);
}

void wk_system_clock_config(void) {
	/* reset crm */
	crm_reset();

	/* enable lick */
	crm_clock_source_enable(CRM_CLOCK_SOURCE_LICK, TRUE);

	/* wait till lick is ready */
	while (crm_flag_get(CRM_LICK_STABLE_FLAG) != SET) {
	}

	/* enable hick */
	crm_clock_source_enable(CRM_CLOCK_SOURCE_HICK, TRUE);

	/* wait till hick is ready */
	while (crm_flag_get(CRM_HICK_STABLE_FLAG) != SET) {
	}

	/* config pll clock resource */
	crm_pll_config(CRM_PLL_SOURCE_HICK, CRM_PLL_MULT_60,
			CRM_PLL_OUTPUT_RANGE_GT72MHZ);

	/* enable pll */
	crm_clock_source_enable(CRM_CLOCK_SOURCE_PLL, TRUE);

	/* wait till pll is ready */
	while (crm_flag_get(CRM_PLL_STABLE_FLAG) != SET) {
	}

	/* config ahbclk */
	crm_ahb_div_set(CRM_AHB_DIV_1);

	/* config apb2clk, the maximum frequency of APB2 clock is 120 MHz  */
	crm_apb2_div_set(CRM_APB2_DIV_2);

	/* config apb1clk, the maximum frequency of APB1 clock is 120 MHz  */
	crm_apb1_div_set(CRM_APB1_DIV_2);

	/* enable auto step mode */
	crm_auto_step_mode_enable(TRUE);

	/* select pll as system clock source */
	crm_sysclk_switch(CRM_SCLK_PLL);

	/* wait till pll is used as system clock source */
	while (crm_sysclk_switch_status_get() != CRM_SCLK_PLL) {
	}

	/* disable auto step mode */
	crm_auto_step_mode_enable(FALSE);

	/* update system_core_clock global variable */
	system_core_clock_update();
}

