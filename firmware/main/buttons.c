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

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "button_gpio.h"
#include "button_types.h"
#include "esp_log.h"
#include "hid.h"
#include "iot_button.h"

static const char *TAG = "button_task";

QueueHandle_t xQueue1;

BaseType_t xButtonQueueReceive(button_id_enum *ButtonID, const TickType_t xTicksToWait) {
  if (!xQueue1 || ButtonID == NULL) {
    return pdFAIL;
  }
  return xQueueReceive(xQueue1, ButtonID, xTicksToWait);
}

BaseType_t xButtonQueueSend(button_id_enum ButtonID, const TickType_t xTicksToWait) {
  if (!xQueue1) {
    return pdFAIL;
  }
  return xQueueSend(xQueue1, &ButtonID, xTicksToWait);
}

void button_single_click_cb(void *arg,void *usr_data)
{
  button_id_enum btn_id = (button_id_enum)(intptr_t)usr_data;
  ESP_LOGI(TAG, "Click event");
  xButtonQueueSend(btn_id, portMAX_DELAY);
}

void buttons_init() {

  // create gpio 02 button
  const button_config_t btn02_cfg = {0};
  const button_gpio_config_t btn02_gpio_cfg = {
    .gpio_num = 2,
    .active_level = 0,
  };
  button_handle_t gpio02_btn = NULL;
  iot_button_new_gpio_device(&btn02_cfg, &btn02_gpio_cfg, &gpio02_btn);
  if(NULL == gpio02_btn) {
    ESP_LOGE(TAG, "Button 02 create failed");
  }

  // create gpio 03 button
  const button_config_t btn03_cfg = {0};
  const button_gpio_config_t btn03_gpio_cfg = {
    .gpio_num = 3,
    .active_level = 0,
  };
  button_handle_t gpio03_btn = NULL;
  iot_button_new_gpio_device(&btn03_cfg, &btn03_gpio_cfg, &gpio03_btn);
  if(NULL == gpio03_btn) {
    ESP_LOGE(TAG, "Button 03 create failed");
  }

  // create gpio 06 button
  const button_config_t btn06_cfg = {0};
  const button_gpio_config_t btn06_gpio_cfg = {
    .gpio_num = 6,
    .active_level = 0,
  };
  button_handle_t gpio06_btn = NULL;
  iot_button_new_gpio_device(&btn06_cfg, &btn06_gpio_cfg, &gpio06_btn);
  if(NULL == gpio06_btn) {
    ESP_LOGE(TAG, "Button 06 create failed");
  }

  iot_button_register_cb(gpio02_btn, BUTTON_PRESS_DOWN, NULL, button_single_click_cb,(void *)(intptr_t)BTN_LEFT);
  iot_button_register_cb(gpio02_btn, BUTTON_LONG_PRESS_HOLD, NULL, button_single_click_cb,(void *)(intptr_t)BTN_LEFT);

  iot_button_register_cb(gpio03_btn, BUTTON_PRESS_DOWN, NULL, button_single_click_cb,(void *)(intptr_t)BTN_RIGHT);
  iot_button_register_cb(gpio03_btn, BUTTON_LONG_PRESS_HOLD, NULL, button_single_click_cb,(void *)(intptr_t)BTN_RIGHT);

  iot_button_register_cb(gpio06_btn, BUTTON_PRESS_DOWN, NULL, button_single_click_cb,(void *)(intptr_t)BTN_ENTER);
  iot_button_register_cb(gpio06_btn, BUTTON_LONG_PRESS_HOLD, NULL, button_single_click_cb,(void *)(intptr_t)BTN_ENTER);

}

void buttonTask( void *pvParameters )
{

  buttons_init();

  xQueue1 = xQueueCreate(10, sizeof( button_id_enum ));
  if (xQueue1 == NULL) {
    ESP_LOGE(TAG, "Queue not created successfully... Fatal error, aborting!");
    abort();
  }

  while (1) {

    vTaskDelay( pdMS_TO_TICKS(1000) );

  }

}