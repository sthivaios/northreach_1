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

#include "button_gpio.h"
#include "button_types.h"
#include "esp_log.h"


static const char *TAG = "button_task";

void buttons_init() {

  // create gpio button
  const button_config_t btn_cfg = {0};
  const button_gpio_config_t btn_gpio_cfg = {
    .gpio_num = 12,
    .active_level = 0,
  };
  button_handle_t gpio_btn = NULL;
  esp_err_t ret = iot_button_new_gpio_device(&btn_cfg, &btn_gpio_cfg, &gpio_btn);
  if(NULL == gpio_btn) {
    ESP_LOGE(TAG, "Button create failed");
  }

}

void buttonTask( void *pvParameters )
{

  buttons_init();

  while (1) {

    ESP_LOGI(TAG, "hi from task loop");

    vTaskDelay( pdMS_TO_TICKS(1000) );

  }

}