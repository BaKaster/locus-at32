#ifndef AT32F403A_407_CONFIG_H
#define AT32F403A_407_CONFIG_H

#include "at32f403a_407_board.h"

void wk_tmr2_init(void)
{
  /* add user code begin tmr2_init 0 */

  /* add user code end tmr2_init 0 */

  gpio_init_type gpio_init_struct;
  tmr_input_config_type  tmr_input_struct;

  gpio_default_para_init(&gpio_init_struct);

  /* add user code begin tmr2_init 1 */

  /* add user code end tmr2_init 1 */

  /* configure the CH1 pin */
  gpio_init_struct.gpio_pins = GPIO_PINS_0;
  gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_UP;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init(GPIOA, &gpio_init_struct);

  /* configure the CH2 pin */
  gpio_init_struct.gpio_pins = GPIO_PINS_1;
  gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_UP;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init(GPIOA, &gpio_init_struct);

  /* configure counter settings */
  tmr_base_init(TMR2, 128, 0);
  tmr_cnt_dir_set(TMR2, TMR_COUNT_UP);
  tmr_clock_source_div_set(TMR2, TMR_CLOCK_DIV4);
  tmr_period_buffer_enable(TMR2, FALSE);

  /* configure primary mode settings */
  tmr_sub_sync_mode_set(TMR2, FALSE);
  tmr_primary_mode_select(TMR2, TMR_PRIMARY_SEL_RESET);

  /* configure encoder mode */
  tmr_input_struct.input_channel_select = TMR_SELECT_CHANNEL_1;
  tmr_input_struct.input_mapped_select = TMR_CC_CHANNEL_MAPPED_DIRECT;
  tmr_input_struct.input_polarity_select = TMR_INPUT_RISING_EDGE;
  tmr_input_struct.input_filter_value = 3;
  tmr_input_channel_init(TMR2, &tmr_input_struct, TMR_CHANNEL_INPUT_DIV_1);

  tmr_input_struct.input_channel_select = TMR_SELECT_CHANNEL_2;
  tmr_input_struct.input_mapped_select = TMR_CC_CHANNEL_MAPPED_DIRECT;
  tmr_input_struct.input_polarity_select = TMR_INPUT_RISING_EDGE;
  tmr_input_struct.input_filter_value = 3;
  tmr_input_channel_init(TMR2, &tmr_input_struct, TMR_CHANNEL_INPUT_DIV_1);

  tmr_encoder_mode_config(TMR2, TMR_ENCODER_MODE_A, TMR_INPUT_RISING_EDGE, TMR_INPUT_RISING_EDGE);

  tmr_counter_enable(TMR2, TRUE);
  /**
   * Users need to configure TMR2 interrupt functions according to the actual application.
   * 1. Call the below function to enable the corresponding TMR2 interrupt.
   *     --tmr_interrupt_enable(...)
   * 2. Add the user's interrupt handler code into the below function in the at32f403a_407_int.c file.
   *     --void TMR2_GLOBAL_IRQHandler(void)
   */

  /* add user code begin tmr2_init 2 */

  /* add user code end tmr2_init 2 */
}


void wk_exint_config(void)
{
  /* add user code begin exint_config 0 */
delay_ms(10);
  /* add user code end exint_config 0 */

  gpio_init_type gpio_init_struct;
  exint_init_type exint_init_struct;

  /* add user code begin exint_config 1 */

  /* add user code end exint_config 1 */

  /* configure the EXINT1 */
  gpio_default_para_init(&gpio_init_struct);
  gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
  gpio_init_struct.gpio_pins = GPIO_PINS_2;
  gpio_init_struct.gpio_pull = GPIO_PULL_UP;
  gpio_init(GPIOC, &gpio_init_struct);

  gpio_exint_line_config(GPIO_PORT_SOURCE_GPIOC, GPIO_PINS_SOURCE2);

  exint_default_para_init(&exint_init_struct);
  exint_init_struct.line_enable = TRUE;
  exint_init_struct.line_mode = EXINT_LINE_INTERRUPUT;
  exint_init_struct.line_select = EXINT_LINE_2;
  exint_init_struct.line_polarity = EXINT_TRIGGER_RISING_EDGE;
  exint_init(&exint_init_struct);

  /**
   * Users need to configure EXINT1 interrupt functions according to the actual application.
   * 1. Call the below function to enable the corresponding EXINT1 interrupt.
   *     --exint_interrupt_enable(EXINT_LINE_1, TRUE);
   * 2. Add the user's interrupt handler code into the below function in the at32f403a_407_int.c file.
   *     --void EXINT1_IRQHandler(void)
   */

  /* add user code begin exint_config 2 */

  /* add user code end exint_config 2 */
}



//void wk_system_clock_config(void)
//{
//  /* reset crm */
//  crm_reset();
//
//  /* enable lick */
//  crm_clock_source_enable(CRM_CLOCK_SOURCE_LICK, TRUE);
//
//  /* wait till lick is ready */
//  while(crm_flag_get(CRM_LICK_STABLE_FLAG) != SET)
//  {
//  }
//
//  /* enable hick */
//  crm_clock_source_enable(CRM_CLOCK_SOURCE_HICK, TRUE);
//
//  /* wait till hick is ready */
//  while(crm_flag_get(CRM_HICK_STABLE_FLAG) != SET)
//  {
//  }
//
//  /* config pll clock resource */
//  crm_pll_config(CRM_PLL_SOURCE_HICK, CRM_PLL_MULT_60, CRM_PLL_OUTPUT_RANGE_GT72MHZ);
//
//  /* enable pll */
//  crm_clock_source_enable(CRM_CLOCK_SOURCE_PLL, TRUE);
//
//  /* wait till pll is ready */
//  while(crm_flag_get(CRM_PLL_STABLE_FLAG) != SET)
//  {
//  }
//
//  /* config ahbclk */
//  crm_ahb_div_set(CRM_AHB_DIV_1);
//
//  /* config apb2clk, the maximum frequency of APB2 clock is 120 MHz  */
//  crm_apb2_div_set(CRM_APB2_DIV_2);
//
//  /* config apb1clk, the maximum frequency of APB1 clock is 120 MHz  */
//  crm_apb1_div_set(CRM_APB1_DIV_2);
//
//  /* enable auto step mode */
//  crm_auto_step_mode_enable(TRUE);
//
//  /* select pll as system clock source */
//  crm_sysclk_switch(CRM_SCLK_PLL);
//
//  /* wait till pll is used as system clock source */
//  while(crm_sysclk_switch_status_get() != CRM_SCLK_PLL)
//  {
//  }
//
//  /* disable auto step mode */
//  crm_auto_step_mode_enable(FALSE);
//
//  /* update system_core_clock global variable */
//  system_core_clock_update();
//}

void wk_tmr3_init(void)
{
  /* add user code begin tmr3_init 0 */

  /* add user code end tmr3_init 0 */


  /* add user code begin tmr3_init 1 */

  /* add user code end tmr3_init 1 */

  /* configure counter settings */
  tmr_base_init(TMR3, 65535, 0);
  tmr_cnt_dir_set(TMR3, TMR_COUNT_UP);
  tmr_clock_source_div_set(TMR3, TMR_CLOCK_DIV1);
  tmr_period_buffer_enable(TMR3, FALSE);

  /* configure primary mode settings */
  tmr_sub_sync_mode_set(TMR3, FALSE);
  tmr_primary_mode_select(TMR3, TMR_PRIMARY_SEL_RESET);

  tmr_counter_enable(TMR3, TRUE);

  /**
   * Users need to configure TMR3 interrupt functions according to the actual application.
   * 1. Call the below function to enable the corresponding TMR3 interrupt.
   *     --tmr_interrupt_enable(...)
   * 2. Add the user's interrupt handler code into the below function in the at32f403a_407_int.c file.
   *     --void TMR3_GLOBAL_IRQHandler(void)
   */

  /* add user code begin tmr3_init 2 */

  /* add user code end tmr3_init 2 */
}


#endif /* AT32F403A_407_CONFIG_H */
