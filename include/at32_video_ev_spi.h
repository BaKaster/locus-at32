/**
  **************************************************************************
  * @file     at32_video_ev_spi.h
  * @brief    at32_video_ev_spi header file
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

#ifndef __AT32_VIDEO_EV_SPI_H
#define __AT32_VIDEO_EV_SPI_H

#include "at32f403a_407_board.h"

#define LCD_SPI_SELECTED				 	           SPI1
#define LCD_SPI_SELECTED_CLK				         CRM_SPI1_PERIPH_CLOCK
#define LCD_SPI_SELECTED_IRQn				        SPI1_IRQn

#define LCD_PEN_PIN                      GPIO_PINS_4 
#define LCD_PEN_MASK                     (1<<4)
#define LCD_PEN_PORT                     GPIOA
#define LCD_PEN_GPIO_CLK			              CRM_GPIOA_PERIPH_CLOCK 
#define PEN_CHECK 	                      gpio_input_data_bit_read(LCD_PEN_PORT,LCD_PEN_PIN)

#define LCD_RST_PIN                      GPIO_PINS_15 
#define LCD_RST_MASK                     (1<<15)
#define LCD_RST_PORT                     GPIOA
#define LCD_RST_GPIO_CLK			              CRM_GPIOA_PERIPH_CLOCK   

#define LCD_BLK_PIN                      GPIO_PINS_9
#define LCD_BLK_MASK                     (1<<9)
#define LCD_BLK_PORT                     GPIOA
#define LCD_BLK_GPIO_CLK			              CRM_GPIOA_PERIPH_CLOCK 

#define LCD_DC_PIN                       GPIO_PINS_7 
#define LCD_DC_MASK                      (1<<7)
#define LCD_DC_PORT                      GPIOC
#define LCD_DC_GPIO_CLK			               CRM_GPIOC_PERIPH_CLOCK 

#define LCD_CS1_PIN                      GPIO_PINS_4
#define LCD_CS1_MASK                     (1<<0)
#define LCD_CS1_PORT                     GPIOA
#define LCD_CS1_GPIO_CLK			              CRM_GPIOA_PERIPH_CLOCK 

#define LCD_CS2_PIN                      GPIO_PINS_4
#define LCD_CS2_MASK                     (1<<1)
#define LCD_CS2_PORT                     GPIOA
#define LCD_CS2_GPIO_CLK			              CRM_GPIOA_PERIPH_CLOCK 

#define LCD_SPI_SCK_PIN			               GPIO_PINS_5
#define LCD_SPI_SCK_PORT			              GPIOA
#define LCD_SPI_SCK_GPIO_CLK			          CRM_GPIOA_PERIPH_CLOCK 
#define LCD_SPI_MOSI_PIN			              GPIO_PINS_7
#define LCD_SPI_MOSI_PORT			             GPIOA
#define LCD_SPI_MOSI_GPIO_CLK			         CRM_GPIOA_PERIPH_CLOCK 
#define LCD_SPI_MISO_PIN			              GPIO_PINS_6
#define LCD_SPI_MISO_PORT			             GPIOA
#define LCD_SPI_MISO_GPIO_CLK			         CRM_GPIOA_PERIPH_CLOCK 

#define	LCD_DC_SET	                      LCD_DC_PORT->scr = LCD_DC_MASK 
#define	LCD_RST_SET	                     LCD_RST_PORT->scr = LCD_RST_MASK
#define	LCD_BLK_SET	                     LCD_BLK_PORT->scr = LCD_BLK_MASK
#define	LCD_CS1_SET	                     LCD_CS1_PORT->scr = LCD_CS1_MASK
#define	LCD_CS2_SET	                     LCD_CS2_PORT->scr = LCD_CS2_MASK

#define	LCD_DC_CLR	                      LCD_DC_PORT->clr = LCD_DC_MASK   
#define	LCD_RST_CLR	                     LCD_RST_PORT->clr = LCD_RST_MASK
#define	LCD_BLK_CLR	                     LCD_BLK_PORT->clr = LCD_BLK_MASK
#define	LCD_CS1_CLR	                     LCD_CS1_PORT->clr = LCD_CS1_MASK
#define	LCD_CS2_CLR	                     LCD_CS2_PORT->clr = LCD_CS2_MASK

#define LCD_SPI_MASTER_DMA               DMA1
#define LCD_SPI_MASTER_DMA_CLK           CRM_DMA1_PERIPH_CLOCK
#define LCD_SPI_MASTER_Tx_DMA_IRQn       DMA1_Channel3_IRQn
#define LCD_SPI_MASTER_Rx_DMA_Channel    DMA1_CHANNEL2
#define LCD_SPI_MASTER_Rx_DMA_INT        DMA1_FDT2_FLAG
#define LCD_SPI_MASTER_Rx_DMA_FLAG       DMA1_FDT2_FLAG
#define LCD_SPI_MASTER_Tx_DMA_Channel    DMA1_CHANNEL3
#define LCD_SPI_MASTER_Tx_DMA_INT        DMA1_FDT3_FLAG
#define LCD_SPI_MASTER_Tx_DMA_FLAG       DMA1_FDT3_FLAG
#define LCD_SPI_MASTER_DR_Base			        (uint32_t)(&(LCD_SPI_SELECTED->dt)); 

extern uint16_t *lcd_data_buf;
extern uint16_t lcd_buffer[];
void lcd_hw_init(void);
uint8_t lcd_spi1_data_receive(void);
void lcd_spi1_write(uint8_t data);
void spi_switch(uint8_t mode);

#endif
