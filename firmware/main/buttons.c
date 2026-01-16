// Copyright 2026 Stratos Thivaios
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "buttons.h"

#include <esp_log.h>
#include <button_gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <iot_button.h>

static const char *TAG = "button_task";

QueueHandle_t button_queue;

button_handle_t btn_up, btn_down, btn_left, btn_right, btn_enter;
button_handle_t btn_vol_up, btn_vol_down;
button_handle_t btn_esc, btn_f5, btn_alt_f4, btn_task_mgr;

static void button_single_click_cb(void *arg,void *usr_data)
{
  const button_id_t button_id = (button_id_t)(int)usr_data;

  ESP_LOGI(TAG, "BUTTON_SINGLE_CLICK");

  button_queue_event_t event_to_send = {
    .event_type = BUTTON_SINGLE_CLICK,
    .button_id = button_id
  };

  xQueueSend(button_queue, &event_to_send, 0);
}

BaseType_t button_queue_receive(button_queue_event_t *button_event,
                                 const TickType_t ticks_to_wait) {
  if (button_queue == NULL || button_event == NULL)
    return pdFAIL;
  return xQueueReceive(button_queue, button_event, ticks_to_wait);
}

void buttons_init() {
  button_queue = xQueueCreate( 50, sizeof( button_queue_event_t ) );

  if( button_queue == NULL )
  {
    ESP_LOGE(TAG, "xQueueCreate() failed. button queue wasn't created. aborting as this is a fatal error.");
    abort();
  }

  const button_config_t btn_cfg = {0};
  const button_gpio_config_t btn_gpio_cfg = {
    .gpio_num = 12,
    .active_level = 0,
  };
  button_handle_t gpio_btn = NULL;
  iot_button_new_gpio_device(&btn_cfg, &btn_gpio_cfg, &gpio_btn);
  if(NULL == gpio_btn) {
    ESP_LOGE(TAG, "Button create failed");
  }

  iot_button_register_cb(gpio_btn, BUTTON_SINGLE_CLICK, NULL, button_single_click_cb,(void*)BTN_UP);

}

QueueHandle_t get_button_queue(void) {
  return button_queue;
}

void buttonTask( void *pvParameters )
{

  buttons_init();

  while (1) {
    vTaskDelay( pdMS_TO_TICKS(1000) );
  }

}