#include "encoder_navigation.h"
#include "at32f403a_407_int.h"

#define ENCODER_MAX 128
// Глобальная переменная для отслеживания состояния кнопки

const uint32_t BUTTON_DEBOUNCE_DELAY = 100; // В миллисекундах
const uint32_t ENCODER_DEBOUNCE_DELAY = 15; // В миллисекундах
static uint32_t last_pressed_time=0;
uint32_t buttonStarttime=0;

static bool encoder_channel_a_last_state = false;
static bool encoder_channel_b_last_state = false;
static uint32_t last_encoder_change_time = 0;
static uint32_t last_encoder_signal_time = 0;
int32_t encoder_delta = 0;
int32_t temp_encoder = 0;

uint8_t scroll_threshold=4;
void encoder_handler(bool updateMenu) {
    // Считываем значение счетчика таймера TMR2
    uint8_t encoder_count = tmr_counter_value_get(TMR2);

    // Проверяем, изменилось ли значение счетчика
    if (encoder_count != temp_encoder) {
        // Вычисляем разницу между текущим и предыдущим значением
        int8_t diff = encoder_count - temp_encoder; // Используем int8_t для корректной работы с переполнением

        // Обновляем значение temp_encoder
        temp_encoder = encoder_count;

        // Обновляем значение encoder_delta
        encoder_delta += diff;

        // Применяем изменения, если накопленная разница превысила порог
        if (abs(encoder_delta) >= scroll_threshold) {
            int16_t new_selected_item = selected_item + (encoder_delta / scroll_threshold); // Используем int16_t для промежуточного значения

            // Обработка переполнения при изменении selected_item:
            if (new_selected_item < 0) {
                selected_item = 0;
            } else if (new_selected_item > current_menu->child_count - 1) {
                selected_item = current_menu->child_count - 1;
            } else {
                selected_item = new_selected_item; // Присваиваем новое значение только если оно в пределах допустимого диапазона
            }

            // Обновляем отображение меню
            if (updateMenu) {
                display_menu(current_menu, selected_item);
            }

            // Сбрасываем накопленное значение encoder_delta
            encoder_delta = 0;
        }
    }
}
void TMR2_GLOBAL_IRQHandler(void) {
  // Сброс флага прерывания
 // at32_led_toggle(LED4);

  bool channel_a_state = gpio_input_data_bit_read(GPIOA, GPIO_PINS_0);
  bool channel_b_state = gpio_input_data_bit_read(GPIOA, GPIO_PINS_1);

  // Проверка на изменение состояния
  if (channel_a_state != encoder_channel_a_last_state || channel_b_state != encoder_channel_b_last_state) {
    // Проверка дебаунсинга
    if (get_millis() - last_encoder_change_time > ENCODER_DEBOUNCE_DELAY) {
      last_encoder_change_time = get_millis();

      // Определение направления вращения
      if (channel_a_state && channel_b_state) {
        // Вращение против часовой стрелки
        encoder_delta--;
      } else if (!channel_a_state && !channel_b_state) {
        // Вращение по часовой стрелке
        encoder_delta++;
      }

      // Обновление последнего состояния каналов
      encoder_channel_a_last_state = channel_a_state;
      encoder_channel_b_last_state = channel_b_state;
    }
  }
  last_encoder_signal_time=get_millis();
  tmr_flag_clear(TMR2, TMR_C2_SWTRIG);
}

void button_handlers()
{

	buttonStarttime = get_millis();

  MenuItem selected = current_menu[selected_item];

  if (selected.type == MENU_TYPE_FOLDER)
  {
    if (selected.child != NULL)
    {
          current_menu = selected.child;
      selected_item = 0;
    }
  } else if (selected.type == MENU_TYPE_ACTION ||
	         selected.type == MENU_TYPE_SLIDER ||
	         selected.type == MENU_TYPE_SWITCH) {
    if (selected.action != NULL) {
      if ((selected.action != Back_func || current_menu != MainMenu))
      {
        selected.action(selected_item);
      }
    }
  }

  display_menu(current_menu, selected_item);
}



void EXINT2_IRQHandler(void)
{
//	at32_led_toggle(LED3);
	uint32_t current = get_millis();
    if(get_millis() - last_pressed_time > 30)
    {

     //   at32_led_toggle(LED3);
       if (get_millis() - functionStartTime > 250)
       {
           last_pressed_time=get_millis();
           functionRunning = !functionRunning;
       }

    }
    exint_flag_clear(EXINT_LINE_2);
}
