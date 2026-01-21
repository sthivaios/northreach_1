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
#include "iot_button.h"

static const char *TAG = "button_task";

QueueHandle_t xButtonEventQueue;
static button_handle_t button_handles_array[10];

// disgusting stupid array for getting a string from the enums to print to the
// log
static const char *button_names[11] = {
    [BTN_UP] = "UP",
    [BTN_DOWN] = "DOWN",
    [BTN_LEFT] = "LEFT",
    [BTN_RIGHT] = "RIGHT",
    [BTN_ENTER] = "ENTER",
    [BTN_VOL_UP] = "VOL_UP",
    [BTN_VOL_DOWN] = "VOL_DOWN",
    [BTN_F1] = "F1",
    [BTN_F2] = "F2",
    [BTN_F3] = "F3",
    [BTN_F4] = "F4",
};

BaseType_t xButtonQueueReceive(button_id_enum *ButtonID,
                               const TickType_t xTicksToWait) {
  if (!xButtonEventQueue || ButtonID == NULL) {
    return pdFAIL;
  }
  return xQueueReceive(xButtonEventQueue, ButtonID, xTicksToWait);
}

BaseType_t xButtonQueueSend(button_id_enum ButtonID,
                            const TickType_t xTicksToWait) {
  if (!xButtonEventQueue) {
    return pdFAIL;
  }
  return xQueueSend(xButtonEventQueue, &ButtonID, xTicksToWait);
}

void button_single_click_cb(void *arg, void *usr_data) {
  const button_id_enum btn_id = (button_id_enum)(intptr_t)usr_data;
  ESP_LOGI(TAG, "Click event from button: %s", button_names[btn_id]);
  xButtonQueueSend(btn_id, portMAX_DELAY);
}

void buttons_init() {

  int i_pin = 2;
  int i_array = 0;

  while (i_pin <= 11) {
    const button_config_t btn_cfg = {0};
    const button_gpio_config_t btn_gpio_cfg = {
        .gpio_num = i_pin,
        .active_level = 0,
    };
    button_handle_t gpio_btn = NULL;
    ESP_ERROR_CHECK(
        iot_button_new_gpio_device(&btn_cfg, &btn_gpio_cfg, &gpio_btn));
    if (NULL == gpio_btn) {
      ESP_LOGE(TAG, "Button [ %d ] create failed", i_pin);
      abort();
    }

    button_handles_array[i_array] = gpio_btn;

    ESP_LOGI(TAG, "Button [ %d ] configured", i_pin);

    i_pin++;
    i_array++;
  }

  const button_id_enum btn_ids[] = {BTN_LEFT, BTN_RIGHT, BTN_UP,
                                    BTN_DOWN, BTN_ENTER, BTN_VOL_UP};

  for (int i = 0; i <= 5; i++) {
    ESP_ERROR_CHECK(iot_button_register_cb(
        button_handles_array[i], BUTTON_PRESS_DOWN, NULL,
        button_single_click_cb, (void *)(intptr_t)btn_ids[i]));
    ESP_ERROR_CHECK(iot_button_register_cb(
        button_handles_array[i], BUTTON_LONG_PRESS_HOLD, NULL,
        button_single_click_cb, (void *)(intptr_t)btn_ids[i]));
    ESP_LOGI(TAG, "Registered callbacks for button [ %d ]", i + 2);
  }
}

void buttonTask(void *pvParameters) {

  buttons_init();

  xButtonEventQueue = xQueueCreate(50, sizeof(button_id_enum));
  if (xButtonEventQueue == NULL) {
    ESP_LOGE(TAG, "The button event queue was not created successfully! This "
                  "is a fatal error. Aborting!");
    abort();
  }

  ESP_LOGW(TAG, "Buttons configured successfully in the button task. The task "
                "will now self delete.");
  vTaskDelete(NULL);
}