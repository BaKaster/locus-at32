#include "stdlib.h"
#include <stdio.h>
#include "string.h"
#include "at32_video_ev_lcd.h"
#include "at32_video_ev_spi.h"

/******************************************************************************/
/* point color and background color */
uint16_t point_color = 0x0000;
uint16_t back_color = 0xFFFF;

lcd_dev_struct lcddev;

// Создаем буфер для хранения пикселей изображения
uint16_t lcd_buffer[LCD_WIDTH * LCD_HEIGHT];

// Функция для заполнения буфера цветом
void lcd_fill_buffer(uint16_t color) {
  for (int i = 0; i < LCD_WIDTH * LCD_HEIGHT; i++) {
    lcd_buffer[i] = color;
  }
}


// Функция для отправки буфера на LCD
void lcd_display_buffer(void) {
	//delay_ms(100);
  lcd_set_window(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);
  lcd_wr_reg(lcddev.wramcmd); // Команда записи в память LCD

  // Здесь мы отправляем содержимое буфера на LCD
  // Если вы используете DMA, настройте его на передачу данных из lcd_buffer
  for (int i = 0; i < LCD_WIDTH * LCD_HEIGHT; i++) {
    lcd_wr_data16(lcd_buffer[i]);
  }
}

// Функция для инициализации LCD и буфера
/**
  * @brief  lcd write register
  * @param  regval: register value
  * @retval none
  */
void lcd_wr_reg(uint8_t regval)
{
  LCD_DC_CLR;//cmd
  lcd_spi1_write(regval);
  LCD_DC_SET;//data
}

/**
  * @brief  lcd write 8bit data
  * @param  data: data value
  * @retval none
  */
void lcd_wr_data(uint8_t data)
{
//  // Подготовка данных для передачи через DMA
//  uint8_t data_array[1] = {data};
//
//  // Настройка DMA для передачи данных
//  LCD_SPI_MASTER_Tx_DMA_Channel->ctrl_bit.mincm = FALSE;
//  LCD_SPI_MASTER_Tx_DMA_Channel->ctrl_bit.chen = FALSE;
//  LCD_SPI_MASTER_Tx_DMA_Channel->dtcnt_bit.cnt = 1; // Передача одного байта данных
//  LCD_SPI_MASTER_Tx_DMA_Channel->maddr  = (uint32_t)data_array;
//  LCD_SPI_MASTER_Tx_DMA_Channel->ctrl_bit.chen = TRUE;
//
//  // Ожидание завершения передачи
//  dma_flag_clear(LCD_SPI_MASTER_Tx_DMA_FLAG);
//
//  // Восстановление настроек SPI
//  LCD_SPI_MASTER_Tx_DMA_Channel->ctrl_bit.mincm = TRUE;
//  spi_frame_bit_num_set(LCD_SPI_SELECTED, SPI_FRAME_8BIT);
	  lcd_spi1_write(data);
}


void lcd_wr_data16(uint16_t data)
{
//  // Подготовка данных для передачи через DMA
//  uint16_t data_array[2] = {(data & 0xFF00) >> 8, data & 0x00FF};
//
//  // Настройка DMA для передачи данных
//  LCD_SPI_MASTER_Tx_DMA_Channel->ctrl_bit.mincm = FALSE;
//  LCD_SPI_MASTER_Tx_DMA_Channel->ctrl_bit.chen = FALSE;
//  LCD_SPI_MASTER_Tx_DMA_Channel->dtcnt_bit.cnt = 2; // Передача двух байт данных
//  LCD_SPI_MASTER_Tx_DMA_Channel->maddr  = (uint32_t)data_array;
//  LCD_SPI_MASTER_Tx_DMA_Channel->ctrl_bit.chen = TRUE;
//
//  // Ожидание завершения передачи
//  dma_flag_clear(LCD_SPI_MASTER_Tx_DMA_FLAG);
//
//  // Восстановление настроек SPI
//  LCD_SPI_MASTER_Tx_DMA_Channel->ctrl_bit.mincm = TRUE;
//  spi_frame_bit_num_set(LCD_SPI_SELECTED, SPI_FRAME_8BIT);
	  lcd_spi1_write(data >> 8);
	  lcd_spi1_write(data & 0xFF);
}


/**
  * @brief  lcd draw a point fast
  * @param  x: x coordinate value
  * @param  y: y coordinate value
  * @param  color: draw color
  * @retval none
  */

void lcd_draw_point(uint16_t x, uint16_t y, uint16_t color) {
  if (x < LCD_WIDTH && y < LCD_HEIGHT) {
    // Рассчитываем индекс в буфере на основе координат x и y
    uint32_t index = y * LCD_WIDTH + x;
    // Записываем цвет в буфер
    lcd_buffer[index] = color;
  }
}

void st7735s_DrawBitmap(Image *img, int x, int y) {
  int i, j;

  if (x < 0 || y < 0 || x + img->w > LCD_WIDTH || y + img->h > LCD_HEIGHT) {
    return;
  }

  for (i = 0; i < img->h; i++) {
    for (j = 0; j < img->w; j++) {
      uint8_t byte1 = img->data[i * img->w * 2 + j * 2];
      uint8_t byte2 = img->data[i * img->w * 2 + j * 2 + 1];

      uint8_t r = (byte2 >> 3) & 0x1F;
      uint8_t g = ((byte2 & 0x07) << 3) | (byte1 >> 5);
      uint8_t b = byte1 & 0x3F;

      // Собираем данные в 16-битный формат RGB565
      uint16_t color = (r << 11) | (g << 5) | b;

      // Записываем данные в буфер LCD
      lcd_draw_point(x + j, y + i, color);
    }
  }
}

// Функция для смешивания цветов с учетом прозрачности (альфа-канала)
uint16_t blend_colors2(uint16_t fg_color, uint16_t bg_color, uint8_t alpha) {
    uint8_t r = (((fg_color >> 11) & 0x1F) * alpha + ((bg_color >> 11) & 0x1F) * (255 - alpha)) / 255;
    uint8_t g = (((fg_color >> 5) & 0x3F) * alpha + ((bg_color >> 5) & 0x3F) * (255 - alpha)) / 255;
    uint8_t b = (((fg_color >> 0) & 0x1F) * alpha + ((bg_color >> 0) & 0x1F) * (255 - alpha)) / 255;

    return (r << 11) | (g << 5) | b;
}

void draw_transparent_png(const Image *img, int x, int y) {

    // Смещение для начала блока альфа-канала
    int alpha_offset = img->w * img->h * 2; // 2 байта на пиксель RGB565

    for (int i = 0; i < img->h; i++) {
        for (int j = 0; j < img->w; j++) {
            // Индекс пикселя в блоке RGB565
            int pixel_index = (i * img->w + j) * 2;

            // Получаем цвет пикселя (RGB565)
            uint16_t color = *(uint16_t*)(img->data + pixel_index);

            // Индекс альфа-канала для текущего пикселя
            int alpha_index = alpha_offset + (i * img->w + j);

            // Получаем значение альфа-канала
            uint8_t alpha = img->data[alpha_index];

            // Смешиваем цвет пикселя с фоном
            uint16_t bg_color = lcd_buffer[(y + i) * LCD_WIDTH + (x + j)];
            uint16_t blended_color = blend_colors2(color, bg_color, alpha);

            // Записываем результирующий цвет в буфер
            lcd_draw_point(x + j, y + i, blended_color);
        }
    }
}



/**
  * @brief  lcd draw a line
  * @param  x1: start x coordinate value
  * @param  y1: start y coordinate value
  * @param  x2: end x coordinate value
  * @param  y2: end y coordinate value
  * @param  color: line color
  * @retval none
  */
void lcd_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
  uint16_t t;
  int xerr = 0, yerr = 0, delta_x, delta_y, distance;
  int incx, incy, urow, ucol;
  delta_x = x2 - x1;
  delta_y = y2 - y1;
  urow = x1;
  ucol = y1;

  if(delta_x > 0)
  {
    incx = 1;
  }
  else if(delta_x == 0)
  {
    incx = 0;  //perpendicular line
  }
  else
  {
    incx = -1;
    delta_x = -delta_x;
  }

  if(delta_y > 0)
  {
    incy = 1;
  }
  else if(delta_y == 0)
  {
    incy = 0;  //horizontal line
  }
  else
  {
    incy = -1;
    delta_y = -delta_y;
  }

  if( delta_x > delta_y)
  {
    distance = delta_x;
  }
  else
  {
    distance = delta_y;
  }

  for(t = 0; t <= distance + 1; t++ )
  {
    lcd_draw_point(urow, ucol, color);
    xerr += delta_x ;
    yerr += delta_y ;

    if(xerr > distance)
    {
      xerr -= distance;
      urow += incx;
    }

    if(yerr > distance)
    {
      yerr -= distance;
      ucol += incy;
    }
  }
}

/**
  * @brief  lcd draw a rectangle
  * @param  x1: start x coordinate value
  * @param  y1: start y coordinate value
  * @param  x2: end x coordinate value
  * @param  y2: end y coordinate value
  * @param  color: rectangle color
  * @retval none
  */
void lcd_draw_rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
  lcd_draw_line(x1, y1, x2, y1, color);
  lcd_draw_line(x1, y1, x1, y2, color);
  lcd_draw_line(x1, y2, x2, y2, color);
  lcd_draw_line(x2, y1, x2, y2, color);
}

/**
  * @brief  lcd draw a circle
  * @param  x0: center of circle x coordinate value
  * @param  y0: center of circle y coordinate value
  * @param  r: radius value
  * @param  color: circle color
  * @retval none
  */
void lcd_draw_circle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color)
{
  int a, b;
  int di;
  a = 0;
  b = r;
  di = 3 - (r << 1);

  while(a <= b)
  {
    lcd_draw_point(x0 + a, y0 - b, color);        //5
    lcd_draw_point(x0 + b, y0 - a, color);        //0
    lcd_draw_point(x0 + b, y0 + a, color);        //4
    lcd_draw_point(x0 + a, y0 + b, color);        //6
    lcd_draw_point(x0 - a, y0 + b, color);        //1
    lcd_draw_point(x0 - b, y0 + a, color);
    lcd_draw_point(x0 - a, y0 - b, color);        //2
    lcd_draw_point(x0 - b, y0 - a, color);        //7
    a++;

    /* draw a circle with bresenham algorithm */
    if(di < 0)
    {
      di += 4 * a + 6;
    }
    else
    {
      di += 10 + 4 * (a - b);
      b--;
    }
  }
}

void lcd_draw_filled_rectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color)
{
  uint16_t i, j;

  // Fill the rectangle with the specified color
  for(i = y; i < y + height; i++)
  {
    for(j = x; j < x + width; j++)
    {
      lcd_draw_point(j, i, color);
    }
  }
}


void lcd_draw_circle_helper(uint16_t x0, uint16_t y0, uint16_t r, uint8_t cornername, uint16_t color)
{
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;

  while (x < r)
  {
    if (f >= 0)
    {
      r--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
    if (cornername & 0x8)
    {
      lcd_draw_point(x0 - r, y0 + x, color);
      lcd_draw_point(x0 - x, y0 + r, color);
    }
    if (cornername & 0x4)
    {
      lcd_draw_point(x0 + x, y0 + r, color);
      lcd_draw_point(x0 + r, y0 + x, color);
    }
    if (cornername & 0x2)
    {
      lcd_draw_point(x0 + r, y0 - x, color);
      lcd_draw_point(x0 + x, y0 - r, color);
    }
    if (cornername & 0x1)
    {
      lcd_draw_point(x0 - x, y0 - r, color);
      lcd_draw_point(x0 - r, y0 - x, color);
    }
  }
}

void lcd_draw_filled_circle_helper(uint16_t x0, uint16_t y0, uint16_t r, uint8_t cornername, uint16_t color)
{
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;

  while (x < r)
  {
    if (f >= 0)
    {
      r--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    // Draw horizontal lines
    if (cornername & 0x8)
    {
      for (int16_t y = y0 - x; y <= y0 + x; y++)
      {
        lcd_draw_point(x0 - r, y, color);
      }
    }
    if (cornername & 0x4)
    {
      for (int16_t y = y0 - x; y <= y0 + x; y++)
      {
        lcd_draw_point(x0 + r, y, color);
      }
    }
    if (cornername & 0x2)
    {
      for (int16_t y = y0 - r; y <= y0 + r; y++)
      {
        lcd_draw_point(x0 + x, y, color);
      }
    }
    if (cornername & 0x1)
    {
      for (int16_t y = y0 - r; y <= y0 + r; y++)
      {
        lcd_draw_point(x0 - x, y, color);
      }
    }
  }
}



void lcd_draw_round_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t r, uint16_t color)
{
  // smarter version
  lcd_draw_line(x + r, y, x + w - r - 1, y, color); // Top
  lcd_draw_line(x + r, y + h - 1, x + w - r - 1, y + h - 1, color); // Bottom
  lcd_draw_line(x, y + r, x, y + h - r - 1, color); // Left
  lcd_draw_line(x + w - 1, y + r, x + w - 1, y + h - r - 1, color); // Right
  // draw four corners
  lcd_draw_circle_helper(x + r, y + r, r, 1, color);
  lcd_draw_circle_helper(x + r, y + h - r - 1, r, 8, color);
  lcd_draw_circle_helper(x + w - r - 1, y + r, r, 2, color);
  lcd_draw_circle_helper(x + w - r - 1, y + h - r - 1, r, 4, color);
}

void lcd_draw_filled_round_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t r, uint16_t color)
{
	if(w<=0)
	{
		return;
	}
  // Заполняем прямоугольник без закругленных углов
  lcd_draw_filled_rectangle(x + r, y, w - r - r, h, color);

  // Рисуем четыре закругленных угла
  lcd_draw_filled_circle_helper(x + r, y + r, r, 0xF, color);
  lcd_draw_filled_circle_helper(x + w - r - 1, y + r, r, 0xF, color);
  lcd_draw_filled_circle_helper(x + r, y + h - r - 1, r, 0xF, color);
  lcd_draw_filled_circle_helper(x + w - r - 1, y + h - r - 1, r, 0xF, color);

  // Соединяем окружности с прямоугольником
  for (int16_t i = 0; i < r; i++) {
    // Левая сторона
    lcd_draw_line(x + i, y + r - i, x + i, y + h - r + i - 1, color);

    // Правая сторона
    lcd_draw_line(x + w - i - 1, y + r - i, x + w - i - 1, y + h - r + i - 1, color);
  }
}

/**
  * @brief  lcd fill circle
  * @param  x0: x coordinate of the center
  * @param  y0: y coordinate of the center
  * @param  r: radius of the circle
  * @param  color: color to fill the circle
  * @retval none
  */
void lcd_fill_circle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color)
{
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  lcd_draw_point(x0, y0 + r, color);
  lcd_draw_point(x0, y0 - r, color);
  lcd_draw_point(x0 + r, y0, color);
  lcd_draw_point(x0 - r, y0, color);
  lcd_draw_filled_rectangle(x0 - r, y0, 2 * r + 1, 1, color);

  while(x < y)
  {
    if(f >= 0)
    {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
    lcd_draw_filled_rectangle(x0 - x, y0 + y, 2 * x + 1, 1, color);
    lcd_draw_filled_rectangle(x0 - x, y0 - y, 2 * x + 1, 1, color);
    lcd_draw_filled_rectangle(x0 - y, y0 + x, 2 * y + 1, 1, color);
    lcd_draw_filled_rectangle(x0 - y, y0 - x, 2 * y + 1, 1, color);
  }
}


/**
  * @brief  lcd show string
  * @param  x: start x coordinate value
  * @param  y: start y coordinate value
  * @param  width: string width
  * @param  height: string height
  * @param  font_t: font
  * @param  p: string point
  * @param  base_color: text color
  */

void lcd_show_string(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const font_t *font, wchar_t *p, const uint16_t base_color)
{
    uint16_t x0 = x;
    uint16_t y0 = y;
    width += x;
    height += y;

    while((*p <= L'ё') && (*p >= L' ')) {

        uint8_t ch = getFontDataIndex(*p, charIndexArray);
    	uint8_t gType = charIndexArray[ch].glyph_type;

    	const font_info *gd = &font->glyph_dsc[ch];
        // Проверяем, не выходим ли за пределы заданной ширины
        if(x + gd->box_w > width) {
            x = x0; // Возврат к началу строки
            y += gd->box_h;  // Переход на следующую строку
        }

        // Проверяем, не выходим ли за пределы заданной высоты
        if(y + gd->box_h > height) {
            break;
        }

        lcd_show_char(x, y+font->size/2, ch, font->glyph_bitmap, font->glyph_dsc, base_color, gType);

        // Сдвигаемся вправо на ширину символа
        x += (gd->adv_w / 16);
        p++;
    }
}


void lcd_show_char(uint16_t x, uint16_t y, uint8_t ch, const uint8_t glyph_bitmap[], const font_info glyph_dsc[], const uint16_t base_color, uint8_t gType) {

    const font_info *gd = &glyph_dsc[ch];

    uint8_t fWidth = gd->box_w;
    uint8_t fHeight = gd->box_h;
    uint32_t bitmap_index = gd->bitmap_index;
    int16_t ofs_x = gd->ofs_x;
    int16_t ofs_y = gd->ofs_y;

    int16_t adjusted_y_offset = 0;

    switch (gType) {
        case 0:
            adjusted_y_offset = -(fHeight - 4);
            break;
        case 1:
            adjusted_y_offset = -fHeight;
            break;
    }

    for (uint8_t j = 0; j < fHeight; j++) {
        int16_t adjusted_y = y + j + adjusted_y_offset - ofs_y;

        for (uint8_t i = 0; i < fWidth; i++) {
            uint32_t glyph_bit_index = j * fWidth + i;
            uint32_t byte_index = glyph_bit_index >> 1;
            uint8_t pixel_value = (glyph_bitmap[bitmap_index + byte_index] >> ((1 - (glyph_bit_index % 2)) * 4)) & 0x0F;

            if (pixel_value != 0) {
                uint16_t bg_color = lcd_buffer[adjusted_y * LCD_WIDTH + (x + i + ofs_x)];
                uint8_t alpha = (pixel_value * 255) / 16; // Быстрое преобразование прозрачности
                uint16_t blended_color = blend_colors(base_color, bg_color, alpha);
                lcd_draw_point(x + i + ofs_x, adjusted_y, blended_color);
            }
        }
    }
}

// Функция для блендинга двух цветов с заданной прозрачностью (alpha)
inline uint16_t blend_colors(uint16_t fg, uint16_t bg, uint8_t alpha) {
    // Улучшенное извлечение и комбинирование цветовых компонент
    uint8_t r = (((fg >> 11) * alpha) + ((bg >> 11) * (255 - alpha))) >> 8;
    uint8_t g = ((((fg >> 5) & 0x3F) * alpha) + (((bg >> 5) & 0x3F) * (255 - alpha))) >> 8;
    uint8_t b = (((fg & 0x1F) * alpha) + ((bg & 0x1F) * (255 - alpha))) >> 8;
    return (r << 11) | (g << 5) | b;
}

/**
  * @brief  write lcd block
  * @param  xstart: x direction start
  * @param  ystart: y direction start
  * @param  xend: x direction end
  * @param  yend: y direction end
  * @retval none
  */
void lcd_set_window(unsigned int Xstart, unsigned int Ystart, unsigned int Xend, unsigned int Yend)
{
  lcd_wr_reg(lcddev.setxcmd);
  lcd_wr_data(Xstart >> 8);
  lcd_wr_data(Xstart & 0xff);
  lcd_wr_data(Xend >> 8);
  lcd_wr_data(Xend & 0xff);

  lcd_wr_reg(lcddev.setycmd);
  lcd_wr_data(Ystart >> 8);
  lcd_wr_data(Ystart & 0xff);
  lcd_wr_data(Yend >> 8);
  lcd_wr_data(Yend & 0xff);

  lcd_wr_reg(lcddev.wramcmd);
}

/**
  * @brief  set lcd cursor position
  * @param  sx: x position
  * @param  sy: y position
  * @retval none
  */
void lcd_set_cursor(uint16_t Xpos, uint16_t Ypos)
{
  lcd_wr_reg(lcddev.setxcmd);
  lcd_wr_data(Xpos >> 8);
  lcd_wr_data(Xpos & 0xff);

  lcd_wr_reg(lcddev.setycmd);
  lcd_wr_data(Ypos >> 8);
  lcd_wr_data(Ypos & 0xff);

  lcd_wr_reg(lcddev.wramcmd);
}

/**
  * @brief  set lcd scan direction
  * @param  dir: direction
  *         0: 0, 1: 90, 2: 180, 3: 270
  * @retval none
  */
void lcd_scan_dir(uint8_t dir)
{
  switch(dir)
  {
    case 0:
      lcddev.width = 128;
      lcddev.height = 160;
      lcddev.setxcmd = 0x2A;
      lcddev.setycmd = 0x2B;
      lcddev.wramcmd = 0x2C;
      lcd_wr_reg(0x36);
      lcd_wr_data(0x00);
      break;
    case 1:
      lcddev.width = 160;
      lcddev.height = 128;
      lcddev.setxcmd = 0x2A;
      lcddev.setycmd = 0x2B;
      lcddev.wramcmd = 0x2C;
      lcd_wr_reg(0x36);
      lcd_wr_data(0xA0);
      break;
    case 2:
      lcddev.width = 128;
      lcddev.height = 160;
      lcddev.setxcmd = 0x2A;
      lcddev.setycmd = 0x2B;
      lcddev.wramcmd = 0x2C;
      lcd_wr_reg(0x36);
      lcd_wr_data(0xC0);
      break;
    case 3:
      lcddev.width = 160;
      lcddev.height = 128;
      lcddev.setxcmd = 0x2A;
      lcddev.setycmd = 0x2B;
      lcddev.wramcmd = 0x2C;
      lcd_wr_reg(0x36);
      lcd_wr_data(0x60);
      break;
    default:
      break;
  }
}

/**
  * @brief  initialize lcd ili9341
  * @param  none
  * @retval none
  */
void st7735s_initial(void)
{
  lcd_wr_reg(0x11);  //sleep out
  delay_ms(10);
  lcd_wr_reg(0xCF);
  lcd_wr_data(0x00);
  lcd_wr_data(0xC1);
  lcd_wr_data(0x30);
  lcd_wr_reg(0xED);
  lcd_wr_data(0x64);
  lcd_wr_data(0x03);
  lcd_wr_data(0x12);
  lcd_wr_data(0x81);
  lcd_wr_reg(0xE8);
  lcd_wr_data(0x85);
  lcd_wr_data(0x00);
  lcd_wr_data(0x79);
  lcd_wr_reg(0xCB);
  lcd_wr_data(0x39);
  lcd_wr_data(0x2C);
  lcd_wr_data(0x00);
  lcd_wr_data(0x34);
  lcd_wr_data(0x02);
  lcd_wr_reg(0xF7);
  lcd_wr_data(0x20);
  lcd_wr_reg(0xEA);
  lcd_wr_data(0x00);
  lcd_wr_data(0x00);
  lcd_wr_reg(0xC0);  //power control
  lcd_wr_data(0xA2); //vrh[5:0]
  lcd_wr_reg(0xC1);  //power control
  lcd_wr_data(0xC5); //sap[2:0];bt[3:0]
  lcd_wr_reg(0xC5);  //vcm control
  lcd_wr_data(0x0E);
  lcd_wr_reg(0x3A);  //memory access control
  lcd_wr_data(0x05);
  lcd_wr_reg(0xB1);
  lcd_wr_data(0x01); // RTNA[3:0] - Set 1-line period
  lcd_wr_data(0x2C); // FPA[5:0] - Front Porch
  lcd_wr_data(0x2D); // BPA[5:0] - Back Porch
  lcd_wr_reg(0xB6);  //display function control
  lcd_wr_data(0x0A);
  lcd_wr_data(0xA2);
  lcd_wr_reg(0x44);
  lcd_wr_data(0x02);
  lcd_wr_reg(0xE0);  //set gamma
  lcd_wr_data(0x02);
   lcd_wr_data(0x1c);
   lcd_wr_data(0x07);
   lcd_wr_data(0x12);
   lcd_wr_data(0x37);
   lcd_wr_data(0x32);
   lcd_wr_data(0x29);
   lcd_wr_data(0x2d);
   lcd_wr_data(0x29);
   lcd_wr_data(0x25);
   lcd_wr_data(0x2B);
   lcd_wr_data(0x39);
   lcd_wr_data(0x00);
   lcd_wr_data(0x01);
   lcd_wr_data(0x03);
   lcd_wr_data(0x10);
  lcd_wr_reg(0xE1);  //set gamma
   lcd_wr_data(0x03);
   lcd_wr_data(0x1d);
   lcd_wr_data(0x07);
   lcd_wr_data(0x06);
   lcd_wr_data(0x2E);
   lcd_wr_data(0x2C);
   lcd_wr_data(0x29);
   lcd_wr_data(0x2D);
   lcd_wr_data(0x2E);
   lcd_wr_data(0x2E);
   lcd_wr_data(0x37);
   lcd_wr_data(0x3F);
   lcd_wr_data(0x00);
   lcd_wr_data(0x00);
   lcd_wr_data(0x02);
   lcd_wr_data(0x10);
  lcd_wr_reg(0x29);  //display on
}

/**
  * @brief  initialize lcd
  * @param  none
  * @retval none
  */
void lcd_init(void)
{
  lcd_hw_init();

  /* lcd reset */
  LCD_RST_CLR;
  delay_ms(100);
  LCD_RST_SET;
  delay_ms(100);

  /* lcd backlight on */
  LCD_BLK_SET;
  delay_ms(100);

  LCD_CS1_CLR;

  st7735s_initial();
  lcd_scan_dir(2);
}

/**
  * @brief  clear lcd
  * @param  color: clear lcd with color
  * @retval none
  */
void lcd_clear(uint16_t color)
{
  // Заполнение буфера заданным цветом
	 uint16_t i, j;
	  for(i = 0; i < LCD_HEIGHT*LCD_WIDTH; i++)
	  {

	       lcd_buffer[i] = color;

	  }
}

/**
  * @brief  fill lcd in designated area with different color
  * @param  sx: x direction start
  * @param  sy: y direction start
  * @param  ex: x direction end
  * @param  ey: y direction end
  * @param  color: fill color point
  * @retval none
  */
void lcd_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color) {
  // Проверка на корректность координат
  if (sx >= LCD_WIDTH || sy >= LCD_HEIGHT || ex > LCD_WIDTH || ey > LCD_HEIGHT || sx >= ex || sy >= ey) {
    return; // Выход, если координаты некорректны
  }

  // Заполнение области пиксель за пикселем
  for (uint16_t y = sy; y < ey; y++) {
    for (uint16_t x = sx; x < ex; x++) {
      lcd_buffer[y * LCD_WIDTH + x] = color;
    }
  }
}
/**
  * @brief  lcd power
  * @param  m: the base of the power
  * @param  n: the exponential of the power
  * @retval result
  */
uint32_t lcd_pow(uint8_t m, uint8_t n)
{
  uint32_t result = 1;

  while(n--)
  {
    result *= m;
  }

  return result;
}

/**
  * @brief  convert bgr to rgb
  * @param  c: convert bgr value
  * @retval rgb value
  */
uint16_t lcd_bgr2rgb(uint16_t c)
{
  uint16_t  r, g, b, rgb;
  b = (c >> 0) & 0x1f;
  g = (c >> 5) & 0x3f;
  r = (c >> 11) & 0x1f;
  rgb = (b << 11) + (g << 5) + (r << 0);
  return(rgb);
}

