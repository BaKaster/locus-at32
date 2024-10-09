/**
  **************************************************************************
  * @file     at32f403a_407_int.c
  * @brief    main interrupt service routines.
  **************************************************************************
  *                       Copyright notice & Disclaimer
  *
  * The software Board Support Package (BSP) that is made available to
  * download from Artery official website is the copyrighted work of Artery.
  * Artery authorizes customers to use, copy, and distribute the BSP
  * software and its related documentation for the purpose of design and
  * development in conjunction with Artery microcontrollers. Use of the
  * software is governed by this copyright notice and the following disclaimer.
  *
  * THIS SOFTWARE IS PROVIDED ON "AS IS" BASIS WITHOUT WARRANTIES,
  * GUARANTEES OR REPRESENTATIONS OF ANY KIND. ARTERY EXPRESSLY DISCLAIMS,
  * TO THE FULLEST EXTENT PERMITTED BY LAW, ALL EXPRESS, IMPLIED OR
  * STATUTORY OR OTHER WARRANTIES, GUARANTEES OR REPRESENTATIONS,
  * INCLUDING BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.
  *
  **************************************************************************
  */

/* includes ------------------------------------------------------------------*/
#include "at32f403a_407_int.h"
#include "at32f403a_407_board.h"
#include <stdbool.h>

volatile uint32_t millis_counter = 0;
volatile bool flag_timeout = false;

uint64_t time_button = 0;
/** @addtogroup AT32F403A_periph_template
  * @{
  */

/** @addtogroup 403A_LED_toggle
  * @{
  */

void init_millis(void)
{
  SysTick_Config(SystemCoreClock / 1000);
  NVIC_SetPriority(SysTick_IRQn, 0);
}

uint32_t get_millis(void)
{
  return millis_counter;
}
/**
  * @brief  this function handles nmi exception.
  * @param  none
  * @retval none
  */
void NMI_Handler(void)
{
}

/**
  * @brief  this function handles hard fault exception.
  * @param  none
  * @retval none
  */
void HardFault_Handler(void)
{
  /* go to infinite loop when hard fault exception occurs */
  while(1)
  {
  }
}

/**
  * @brief  this function handles memory manage exception.
  * @param  none
  * @retval none
  */
void MemManage_Handler(void)
{
  /* go to infinite loop when memory manage exception occurs */
  while(1)
  {
  }
}

/**
  * @brief  this function handles bus fault exception.
  * @param  none
  * @retval none
  */
void BusFault_Handler(void)
{
  /* go to infinite loop when bus fault exception occurs */
  while(1)
  {
  }
}

/**
  * @brief  this function handles usage fault exception.
  * @param  none
  * @retval none
  */
void UsageFault_Handler(void)
{
  /* go to infinite loop when usage fault exception occurs */
  while(1)
  {
  }
}

/**
  * @brief  this function handles svcall exception.
  * @param  none
  * @retval none
  */
void SVC_Handler(void)
{
}

/**
  * @brief  this function handles debug monitor exception.
  * @param  none
  * @retval none
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  this function handles pendsv_handler exception.
  * @param  none
  * @retval none
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  this function handles systick handler.
  * @param  none
  * @retval none
  */
void SysTick_Handler(void)
{
	 millis_counter++;
}



#define DELAY_TIMER TMR3

// Тактовая частота таймера (установите фактическое значение)
#define DELAY_TIMER_CLOCK_FREQ 1000000 // 1 МГц

void delay_unblocked(uint32_t ms) {
  // Вычисляем количество переполнений таймера для нужной задержки
  uint32_t overflow_count = ms / 65; // 65 мс - период по умолчанию

  // Если нужно меньше 65 мс, дожидаемся нужного количества тактов
  uint32_t remaining_counts = (DELAY_TIMER_CLOCK_FREQ / 1000) * (ms % 65);

  // Сбрасываем флаг тайм-аута
  flag_timeout = false;

  // Устанавливаем период (только если нужно больше 65 мс)
  if (overflow_count > 0) {
    tmr_period_value_set(DELAY_TIMER, 65535);
  }

  tmr_interrupt_enable(DELAY_TIMER, TMR_OVF_INT, TRUE);
  // Запускаем таймер (если он еще не запущен)
  if (!tmr_flag_get(DELAY_TIMER, TMR_OVF_INT)) {
    tmr_counter_enable(DELAY_TIMER, TRUE);
  }

  // Ожидаем нужное количество переполнений
  while (overflow_count > 0) {
    while (!flag_timeout); // Ждем переполнения
    flag_timeout = false; // Сбрасываем флаг
    overflow_count--;
  }

  // Если нужно меньше 65 мс, дожидаемся оставшихся тактов
  if (remaining_counts > 0) {
    uint32_t start_count = tmr_counter_value_get(DELAY_TIMER);
    while (tmr_counter_value_get(DELAY_TIMER) - start_count < remaining_counts);
  }
}

// Обработчик прерывания по переполнению таймера
void TMR3_GLOBAL_IRQHandler(void) {
  if (tmr_flag_get(DELAY_TIMER, TMR_OVF_FLAG) != RESET) {
    tmr_flag_clear(DELAY_TIMER, TMR_OVF_FLAG);
    flag_timeout = true;
  }
}

void EXINT3_IRQHandler(void) {
	if (EXINT->intsts & EXINT_LINE_3) {
		check_button_mn(1);
		exint_flag_clear(EXINT_LINE_3);
	}
}

void EXINT4_IRQHandler(void) {
	if (EXINT->intsts & EXINT_LINE_4) {
		check_button_mn(2);
		exint_flag_clear(EXINT_LINE_4);
	}

}

uint64_t get_time() {
	return time_button;
}


void TMR6_GLOBAL_IRQHandler(void) {
	TMR6->ists_bit.ovfif = 0;

}

void TMR7_GLOBAL_IRQHandler(void) {
	TMR7->ists_bit.ovfif = 0;
	time_button++;

}

/**
  * @}
  */

/**
  * @}
  */
