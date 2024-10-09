#include "at32_video_ev_spi.h"

uint16_t *lcd_data_buf;

/**
  * @brief  initialize lcd spi1
  * @param  none
  * @retval none
  */
void lcd_hw_init(void)
{ 
  gpio_init_type	gpio_init_struct;
  dma_init_type  dma_init_struct; 
  spi_init_type  spi_init_struct; 
 
  gpio_pin_remap_config(SWJTAG_GMUX_010, TRUE);
 
  gpio_default_para_init(&gpio_init_struct);
  gpio_init_struct.gpio_pins = LCD_BLK_PIN;      //LCD_BLK
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
  gpio_init_struct.gpio_drive_strength =  GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init(LCD_BLK_PORT, &gpio_init_struct);
 
  gpio_init_struct.gpio_pins = LCD_RST_PIN;      //LCD_RST
  gpio_init(LCD_RST_PORT, &gpio_init_struct);

  gpio_init_struct.gpio_pins = LCD_DC_PIN; 		    //LCD_DC
  gpio_init(LCD_DC_PORT, &gpio_init_struct);

  gpio_init_struct.gpio_pins = LCD_CS1_PIN; 		   //LCD_CS1
  gpio_init(LCD_CS1_PORT, &gpio_init_struct);

  gpio_init_struct.gpio_pins = LCD_CS2_PIN;      //LCD_CS2
  gpio_init(LCD_CS2_PORT, &gpio_init_struct);
 
  LCD_BLK_CLR;
  LCD_RST_SET;
  LCD_CS1_SET;
  LCD_CS2_SET;

  /* configure spi_master pins: sck, mosi and miso */
  /* configure miso pin as alternate function push pull */
  gpio_default_para_init(&gpio_init_struct);
  gpio_init_struct.gpio_pins = LCD_SPI_SCK_PIN;      //LCD_SPI_SCK
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_drive_strength =  GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init(LCD_SPI_SCK_PORT, &gpio_init_struct);

  gpio_init_struct.gpio_pins = LCD_SPI_MOSI_PIN;     //LCD_SPI_MOSI
  gpio_init(LCD_SPI_MOSI_PORT, &gpio_init_struct);

  /* configureand mosi pins as alternate function push pull */
  gpio_init_struct.gpio_pins = LCD_SPI_MISO_PIN;     //LCD_SPI_MISO
  gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
  gpio_init_struct.gpio_pull = GPIO_PULL_DOWN;
  gpio_init(LCD_SPI_MISO_PORT, &gpio_init_struct);

  /* spi_master configuration */
  spi_default_para_init(&spi_init_struct);
  spi_init_struct.transmission_mode = SPI_TRANSMIT_FULL_DUPLEX;
  spi_init_struct.master_slave_mode = SPI_MODE_MASTER;
  spi_init_struct.mclk_freq_division = SPI_MCLK_DIV_2;
  spi_init_struct.first_bit_transmission = SPI_FIRST_BIT_MSB;
  spi_init_struct.frame_bit_num = SPI_FRAME_8BIT;
  spi_init_struct.clock_polarity = SPI_CLOCK_POLARITY_HIGH;
  spi_init_struct.clock_phase = SPI_CLOCK_PHASE_2EDGE;
  spi_init_struct.cs_mode_selection = SPI_CS_SOFTWARE_MODE;
  spi_init(LCD_SPI_SELECTED, &spi_init_struct);
  /* enable spi_maser tx request */
  spi_i2s_dma_transmitter_enable(LCD_SPI_SELECTED, TRUE);
  /* enable spi_maser */
  spi_enable(LCD_SPI_SELECTED, TRUE);

  /* lcd_spi_master_tx_dma_channel configuration */
  dma_reset(LCD_SPI_MASTER_Tx_DMA_Channel);
  dma_init_struct.buffer_size = 128*160;
  dma_init_struct.direction = DMA_DIR_MEMORY_TO_PERIPHERAL;
  dma_init_struct.memory_base_addr = (uint32_t)lcd_buffer;
  dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_HALFWORD;
  dma_init_struct.memory_inc_enable = TRUE;
  dma_init_struct.peripheral_base_addr = (uint32_t)LCD_SPI_MASTER_DR_Base;
  dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_BYTE;
  dma_init_struct.peripheral_inc_enable = FALSE;
  dma_init_struct.priority = DMA_PRIORITY_HIGH;
  dma_init_struct.loop_mode_enable = FALSE;
  dma_init(LCD_SPI_MASTER_Tx_DMA_Channel, &dma_init_struct);

  nvic_irq_enable(LCD_SPI_MASTER_Tx_DMA_IRQn, 0, 2);
 //dma_interrupt_enable(LCD_SPI_MASTER_Tx_DMA_Channel,DMA_FDT_INT,TRUE);
}

/**
  * @brief  lcd spi1 receive data
  * @param  none
  * @retval receive data
  */
uint8_t lcd_spi1_data_receive(void)
{
  uint32_t retry = 0;
  spi_i2s_flag_clear(LCD_SPI_SELECTED, SPI_I2S_RDBF_FLAG);
  spi_i2s_data_transmit(LCD_SPI_SELECTED, 0);
  while (spi_i2s_flag_get(LCD_SPI_SELECTED, SPI_I2S_RDBF_FLAG) == RESET)
  {
    retry++;
    if(retry > 0xFFFFF)
    {
      return 0;
    }
  }
  return (uint8_t)spi_i2s_data_receive(LCD_SPI_SELECTED);
}

/**
  * @brief  lcd spi1 write
  * @param  data: write data
  * @retval read data
  */
void lcd_spi1_write(uint8_t data)
{
  uint32_t retry = 0;
  while (spi_i2s_flag_get(LCD_SPI_SELECTED, SPI_I2S_TDBE_FLAG) == RESET)
  {
    retry++;
    if(retry > 10000)
      return;
  }
  spi_i2s_data_transmit(LCD_SPI_SELECTED, data);
  retry = 0;
  while (spi_i2s_flag_get(LCD_SPI_SELECTED, SPI_I2S_BF_FLAG) == SET)
  {
    retry++;
    if(retry > 0xFFFFF)
      return;
  }
}

/**
  * @brief  spi mode switch between lcd and touch
  * @param  mode 
  *         1:touch, 0:lcd
  * @retval none
  */
void spi_switch(uint8_t mode)
{
  if(mode)
  {
    LCD_CS1_SET;
    spi_enable(LCD_SPI_SELECTED, FALSE);
    LCD_SPI_SELECTED->ctrl1_bit.mdiv_l = SPI_MCLK_DIV_32;
    spi_enable(LCD_SPI_SELECTED, TRUE);
    LCD_CS2_CLR;
  }
  else
  {
    LCD_CS2_SET;
    spi_enable(LCD_SPI_SELECTED, FALSE);
    LCD_SPI_SELECTED->ctrl1_bit.mdiv_l = SPI_MCLK_DIV_4;
    spi_enable(LCD_SPI_SELECTED, TRUE);
    LCD_CS1_CLR;
  }

}
