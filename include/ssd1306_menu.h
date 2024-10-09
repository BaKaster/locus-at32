#ifndef SSD1306_MENU_H
#define SSD1306_MENU_H

#include "at32f403a_407_board.h"
#include "at32f403a_407_int.h"
#include <stdbool.h>
#include "images.h"
#include "encoder_navigation.h"
#include "menu_functions.h"
// Определения типов пунктов меню
#define MENU_TYPE_FOLDER 1
#define MENU_TYPE_ACTION 2
#define MENU_TYPE_SLIDER 3
#define MENU_TYPE_SWITCH 4

// Максимальная длина имени пункта меню
#define MAX_ITEM_LENGTH 20

// Определения позиций меню
#define MENU_TOP_POSITION 0
#define MENU_MIDDLE_POSITION 21
#define MENU_BOTTOM_POSITION 42

typedef enum {
  DATA_TYPE_BOOL,
  DATA_TYPE_UINT8,
  DATA_TYPE_UINT16,
  DATA_TYPE_WCHART
} DataType;

typedef struct {
  wchar_t info_name[MAX_ITEM_LENGTH];
  DataType data_type;
  union {
    bool bool_val;
    uint8_t uint8_val;
    uint16_t uint16_val;
    wchar_t wchart_val;
  } data;
} InfoItem;

typedef struct {
  int min_value;  // Минимальное значение слайдера
  int max_value;  // Максимальное значение слайдера
  uint8_t current_value; // Текущее значение слайдера
} SliderData;

typedef struct {
	bool state;
	 wchar_t info[MAX_ITEM_LENGTH];
} SwitchData;

// Структура для пункта меню
typedef struct MenuItem {
  uint8_t type;
  wchar_t name[MAX_ITEM_LENGTH];
  void (*action)(uint8_t selected_item);
  struct MenuItem *child;
  struct MenuItem *parent;
  uint8_t child_count;
  const Image *icon;
  uint8_t parent_num;
  union {
      SliderData *slider_data;
      SwitchData *switch_data;
    } data;
} MenuItem;


extern SliderData brightness_data;
extern SwitchData inversion_state;
// Глобальные переменные для меню
extern int16_t scroll_offset;
extern uint8_t selected_item;
extern MenuItem *current_menu;
extern MenuItem *parent_menu;
extern MenuItem MainMenu[];
extern MenuItem FolderMenu[];
extern MenuItem SettingsMenu[];
extern MenuItem* active_menu_item;
// Объявления функций для работы с меню
uint8_t calculate_items_to_display(MenuItem *menu, uint8_t total_items);
void menu_init(void);
void Back_func(void);
void Brightness_selected(uint8_t selected_item);
void display_menu_item(MenuItem *item, uint8_t position, int8_t start_item);
void display_scroll_bar(uint8_t total_items, uint8_t selected_item, uint8_t visible_items,  bool scroll_on_hover);
void display_menu(MenuItem *menu, uint8_t selected_item);

#endif /* SSD1306_MENU_H */
