#include "ssd1306_menu.h"


MenuItem *current_menu;
MenuItem MainMenu[6];
MenuItem DisplayMenu[4];
MenuItem SettingsMenu[3];
uint8_t selected_item;

const uint16_t lcd_width = LCD_WIDTH;
const uint16_t lcd_height = LCD_HEIGHT;

SliderData brightness_data = {0, 100, 50};
SwitchData inversion_state = {false, L"Выкл."};

#define OFFSET 27
#define BUTTON_HEIGHT 23

const uint16_t SCROLL_BAR_HEIGHT = LCD_HEIGHT;

void Back_func() {
	  selected_item = current_menu->parent_num;
  current_menu = current_menu->parent;
  // Сбрасываем active_menu_item при выходе из подменю
  active_menu_item = NULL;
}

int16_t menu_y_offset = 0; // Global variable to store the offset
MenuItem* active_menu_item = NULL;


void display_status_bar() { //оптимизировано
  // Используем lcd_draw_filled_rectangle для быстроты
  lcd_draw_filled_rectangle(0, 0, 128, 21, STATUS_BAR);

  lcd_show_string(3, 5, 120, 100, &Montserrat_12, L">", WHITE);

  const wchar_t* text = NULL;
  if (active_menu_item) {
    text = active_menu_item->name;
  } else if (current_menu == MainMenu && current_menu->type != MENU_TYPE_FOLDER) {

    st7735s_DrawBitmap(&Geoscan_logo, 3, 4);
  } else {
    text = current_menu->parent ?
           current_menu->parent[current_menu->parent_num].name :
           current_menu->name;
  }

  if (text) {
    lcd_show_string(10, 5, 120, 100, &Montserrat_12, text, WHITE);
  }

  lcd_show_string(88, 5, 120, 100, &Montserrat_12, L"100%", WHITE);
}

inline void display_menu_item(MenuItem *item, uint8_t position, int8_t start_item) { //вроде оптимизировано
  // Предрасчет значений для оптимизации
  int16_t highlighted_item = position + start_item;
  uint16_t y_pos = 25 + position * OFFSET + menu_y_offset;
  uint16_t x_pos = (item->icon != NULL) ? 27 : 6; // Смещение текста, если нет иконки

  // Выбор цвета фона и рамки
  uint16_t bg_color = (highlighted_item == selected_item) ?
                      (isEditing ? EDIT_MODE_BG : BRIGHTED_SELECT) :
                      BUTTONS;
  uint16_t border_color = (highlighted_item == selected_item) ? WHITE : 0x0000; //

  // Отрисовка фона и рамки одним блоком
  if (item->type == MENU_TYPE_SLIDER || item->type == MENU_TYPE_SWITCH) {
    lcd_draw_filled_round_rect(2, y_pos, 121, BUTTON_HEIGHT * 2, 7, bg_color);
    if (border_color != 0x0000) {
      lcd_draw_round_rect(2, y_pos, 121, BUTTON_HEIGHT * 2, 7, border_color);
    }
    menu_y_offset += BUTTON_HEIGHT;
  } else {
    lcd_draw_filled_round_rect(2, y_pos, 121, BUTTON_HEIGHT, 7, bg_color);
    if (border_color != 0x0000) {
      lcd_draw_round_rect(2, y_pos, 121, BUTTON_HEIGHT, 7, border_color);
    }
  }

  // Рисуем иконку
  if (item->icon != NULL) {
    draw_transparent_png(item->icon, 4, y_pos + 1);
  }

  // Выводим текст пункта меню
  lcd_show_string(x_pos, y_pos + 5, 120, 100, &Montserrat_16, item->name, WHITE);

  // Отрисовка слайдера
  if (item->type == MENU_TYPE_SLIDER) {
	uint16_t slider_width = 112;
    uint16_t slider_x = 6;
    uint16_t slider_y = y_pos + 26;
    int16_t min_value = item->data.slider_data->min_value;
    int16_t max_value = item->data.slider_data->max_value;
    // Оптимизация вычисления позиции ползунка
    uint16_t thumb_pos = slider_x + (((item->data.slider_data->current_value - min_value) * slider_width) / (max_value - min_value));
    thumb_pos = (thumb_pos < slider_x) ? slider_x : ((thumb_pos > slider_x + slider_width) ? (slider_x + slider_width) : thumb_pos);
    uint16_t fill_width = thumb_pos - slider_x;

    // Оптимизация отрисовки слайдера
    lcd_draw_filled_round_rect(slider_x, slider_y, slider_width, 12, 3, WHITE); // Фон слайдера
    if (fill_width > 6) {
      lcd_draw_filled_round_rect(slider_x, slider_y, fill_width, 12, 3, GEOSCAN_COLOR); // Заливка слайдера
    }
  }

  // Отрисовка переключателя
  if (item->type == MENU_TYPE_SWITCH) {
    uint8_t slider_x = 6;
    uint8_t slider_y = y_pos + 22;

    // Убираем лишние комментарии и используем тернарный оператор для выбора цвета
    lcd_draw_filled_round_rect(slider_x, slider_y, 36, 19, 7,
    		item->data.switch_data->state ? GEOSCAN_COLOR : SWITCH_OFF);
    lcd_fill_circle(slider_x + (item->data.switch_data->state ? 26 : 9), slider_y + 9, 7, SWITCH_CIRCLE);
    lcd_show_string(slider_x + 40, slider_y + 5, 120, 100, &Montserrat_12, item->data.switch_data->state ? L"Вкл." : L"Выкл.", WHITE);
  }
}

void display_scroll_bar(uint8_t total_items, uint8_t selected_item, uint8_t visible_items, bool scroll_on_hover) { //Оптимизировано
  // Если общее количество элементов меньше или равно видимому, отображаем только фон
  if (total_items <= visible_items) {
    lcd_draw_filled_rectangle(126, 10, 128, SCROLL_BAR_HEIGHT, SCROLL_BAR);
    return;
  }

  // Вычисляем высоту ползунка, ограничивая ее снизу 10 пикселями
  // Используем умножение и сдвиг вместо деления для ускорения
  uint16_t thumb_height = (SCROLL_BAR_HEIGHT * visible_items) / total_items;
  thumb_height = (thumb_height < 10) ? 10 : thumb_height;

  // Вычисляем позицию ползунка
  uint16_t scroll_y_position = 0;
  if (scroll_on_hover) {
    // Скролл при наведении: ползунок следует за выбранным элементом
    if (selected_item >= visible_items) {
      // Используем умножение и сдвиг вместо деления для ускорения
      scroll_y_position = ((SCROLL_BAR_HEIGHT * (selected_item - visible_items + 1)) / (total_items - visible_items + 1));
    }
  } else {
    // Скролл при изменении пункта: плавное движение ползунка
    // Используем умножение и сдвиг вместо деления для ускорения
    scroll_y_position = (((SCROLL_BAR_HEIGHT - thumb_height) * selected_item) / (total_items - visible_items));
  }

  // Используем условное выражение вместо if для ускорения
  scroll_y_position = (scroll_y_position > (SCROLL_BAR_HEIGHT - thumb_height)) ? (SCROLL_BAR_HEIGHT - thumb_height) : scroll_y_position;

  lcd_draw_filled_rectangle(126, 10, 128, SCROLL_BAR_HEIGHT, BUTTONS);
  lcd_draw_filled_rectangle(126, 10 + scroll_y_position, 128, thumb_height, SCROLL_BAR);
}

void display_menu(MenuItem *menu, uint8_t selected_item) { //вроде оптимизировано
	lcd_clear(BACKGROUND);

  uint8_t total_items = menu->child_count;
  uint8_t items_to_display = calculate_items_to_display(menu, total_items);
  // Определение начального пункта для отображения
  int8_t start_item = selected_item - (items_to_display - 1);
  start_item = (start_item < 0) ? 0 : ((start_item > total_items - items_to_display) ? (total_items - items_to_display) : start_item);

  menu_y_offset = 0;


  // Отображение пунктов меню
  for (int8_t i = 0; i < items_to_display; i++) {
    if ((start_item + i) < total_items) {
      display_menu_item(&(menu[start_item + i]), i, start_item);
    }
  }
  display_scroll_bar(total_items, selected_item, items_to_display, 1);
  display_status_bar();

	lcd_display_buffer();
}

inline uint8_t calculate_items_to_display(MenuItem *menu, uint8_t total_items) { //оптимизировано
  // Используем unsigned для предотвращения предупреждений о сравнении знаковых и беззнаковых типов
  uint8_t max_items = 5;
  uint16_t current_height = 25;

  // Объединяем проверку условия в цикле for
  for (uint8_t i = 0; i < total_items && current_height <= LCD_HEIGHT; ++i) {
    current_height += (menu[i].type == MENU_TYPE_SLIDER || menu[i].type == MENU_TYPE_SWITCH) ?
                       (BUTTON_HEIGHT * 2) :
                       BUTTON_HEIGHT;
    if (current_height > LCD_HEIGHT) {
      max_items = i;
      break; // Выходим из цикла, как только превысили высоту экрана
    }
  }
  return max_items;
}

// Функция для инициализации меню
void menu_init() {
  // Инициализация подменю

	SettingsMenu[0] = (MenuItem){MENU_TYPE_ACTION, L"Назад", Back_func, NULL, MainMenu, 3, &Back, 2, NULL};
	SettingsMenu[1] = (MenuItem){MENU_TYPE_ACTION, L"Сеть", NULL, NULL, MainMenu, 3, &Network, 2, NULL};
	SettingsMenu[2] = (MenuItem){MENU_TYPE_FOLDER, L"Экран", NULL, DisplayMenu, MainMenu, 3, &Display, 2, NULL};

	DisplayMenu[0] = (MenuItem){MENU_TYPE_ACTION, L"Назад", Back_func, NULL, SettingsMenu, 4, &Back, 2, NULL};
	DisplayMenu[1] = (MenuItem){MENU_TYPE_SLIDER, L"Яркость:", Brightness_selected, NULL, SettingsMenu, 4, &Display, 2,  &brightness_data };
	DisplayMenu[2] = (MenuItem){MENU_TYPE_SWITCH, L"Инверсия:", color_inversion, NULL, SettingsMenu, 4, NULL, 2, &inversion_state};
	DisplayMenu[3] = (MenuItem){MENU_TYPE_ACTION, L"Таймер", NULL, NULL, SettingsMenu, 4, NULL, 2, NULL};
		  // Инициализация главного меню
	MainMenu[0] = (MenuItem){MENU_TYPE_ACTION, L"Сведения", Information, NULL, NULL, 6, &Info, 0, NULL};
	MainMenu[1] = (MenuItem){MENU_TYPE_ACTION, L"Режим сна", Screen_off, NULL, NULL, 6, &Power, 0};
	MainMenu[2] = (MenuItem){MENU_TYPE_FOLDER, L"Настройки", NULL, SettingsMenu, NULL, 6, &Settings, 0, NULL};
	MainMenu[3] = (MenuItem){MENU_TYPE_ACTION, L"Геоскан", Geoscan_logotype, NULL, NULL, 6, &Geoscan, 0, NULL};
	MainMenu[4] = (MenuItem){MENU_TYPE_ACTION, L"Ввод", input_number_handler, NULL, NULL, 6, &Input, 0, NULL};
	MainMenu[5] = (MenuItem){MENU_TYPE_ACTION, L"Функция", NULL, NULL, NULL, 6, &Function, 0, NULL};



  // Установка текущего меню на главное меню
  current_menu = MainMenu;
  selected_item = 0;

}

