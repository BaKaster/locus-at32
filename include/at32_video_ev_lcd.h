#ifndef __AT32_VIDEO_EV_LCD_H
#define __AT32_VIDEO_EV_LCD_H
#include "at32f403a_407_board.h"
#include <stdbool.h>

#define LCD_WIDTH            	           128
#define LCD_HEIGHT           	           160

extern const uint16_t lcd_width;
extern const uint16_t lcd_height;

typedef struct
{
  uint16_t width;			  //lcd width
  uint16_t height;		  //lcd height
  uint16_t id;				    //lcd id
  uint8_t dir;			     //lcd direction
  uint16_t	wramcmd;	  //write ram cmd
  uint16_t setxcmd;   //set x coordinate cmd
  uint16_t setycmd;   //set y coordinate cmd
} lcd_dev_struct;

typedef struct {
    wchar_t character;
    int dataIndex;
    int glyph_type;
} CharIndex;

typedef struct {
    uint32_t bitmap_index;          /**< Start index of the bitmap. A font can be max 4 GB.*/
    uint32_t adv_w;                 /**< Draw the next glyph after this width. 28.4 format (real_value * 16 is stored).*/
    uint16_t box_w;                 /**< Width of the glyph's bounding box*/
    uint16_t box_h;                 /**< Height of the glyph's bounding box*/
    int16_t ofs_x;                  /**< x offset of the bounding box*/
    int16_t ofs_y;                  /**< y offset of the bounding box. Measured from the top of the line*/
} font_info;

typedef struct {
    const uint8_t *glyph_bitmap; // Указатель на растровые данные шрифта
    const font_info *glyph_dsc;  // Указатель на массив описаний глифов
    uint8_t size;
} font_t;

typedef struct {
  const uint8_t *data;
  int w;
  int h;
} Image;

extern const uint8_t Montserrat_16_bitmap[];
extern const uint8_t Montserrat_12_bitmap[];

extern CharIndex charIndexArray[];

extern const font_info Montserrat_16_glf[];
extern const font_info Montserrat_12_glf[];

extern const font_t Montserrat_16;
extern const font_t Montserrat_12;

extern lcd_dev_struct lcddev;
/* point color and background color */
extern uint16_t point_color;
extern uint16_t back_color;


#define DFT_SCAN_DIR                     L2R_U2D

/* color value */
#define WHITE         	                  0xFFFF
#define BLACK         	                  0x0000
#define BLUE         	                  0x001F
#define RED           	                  0xF800
#define GREEN         	                  0x07E0
#define CYAN          	                  0x7FFF
#define YELLOW        	                  0xFFE0
#define BROWN 			                  0xBC40
#define GRAY  			                  0x8430

#define BUTTONS 	                      0x3186
#define STATUS_BAR 	                      0x18E3
#define GEOSCAN_COLOR 	                  0xC201
#define BRIGHTED_SELECT                   0X52AA
#define BACKGROUND                        0x1082
#define SCROLL_BAR                        0x7BCF
#define SWITCH_OFF                        0x8C51
#define SWITCH_CIRCLE                     0xDEDB
#define EDIT_MODE_BG                      0x434E

extern volatile bool dma_transfer_complete;
uint8_t calculate_alpha(int16_t x, int16_t y, uint16_t r);
void draw_transparent_png(const Image *img, int x, int y);
int16_t getFontDataIndex(wchar_t ch, const CharIndex* charIndexArray);
uint16_t blend_colors(uint16_t fg, uint16_t bg, uint8_t alpha);
void st7735s_DrawBitmap(Image *img, int x, int y);
void lcd_draw_round_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t r, uint16_t color);
void lcd_display_buffer(void);
void lcd_wr_reg(uint8_t regval);
void lcd_wr_data(uint8_t data);
void lcd_wr_data16(uint16_t data);
void lcd_draw_point(uint16_t x, uint16_t y, uint16_t color);
void lcd_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void lcd_draw_rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void lcd_draw_filled_round_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t r, uint16_t color);
void lcd_draw_circle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color);
void lcd_draw_circle_helper(uint16_t x0, uint16_t y0, uint16_t r, uint8_t cornername, uint16_t color);
void lcd_show_string(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const font_t *font, wchar_t *p, const uint16_t base_color);
void lcd_show_char(uint16_t x, uint16_t y, uint8_t ch, const uint8_t glyph_bitmap[], const font_info glyph_dsc[], const uint16_t base_color, uint8_t gType);
void lcd_set_window(unsigned int Xstart, unsigned int Ystart, unsigned int Xend, unsigned int Yend);
void lcd_set_cursor(uint16_t Xpos, uint16_t Ypos);
void lcd_scan_dir(uint8_t dir);
void st7735s_initial(void);
void lcd_init(void);
void lcd_clear(uint16_t color);
void lcd_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color);
uint32_t lcd_pow(uint8_t m, uint8_t n);
uint16_t lcd_bgr2rgb(uint16_t c);

#endif

