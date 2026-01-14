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

#include "esp_log.h"
#include <FreeRTOS.h>
#include <freertos/queue.h>

static const char *TAG = "button_task";

QueueHandle_t button_queue;

void buttons_init() {
  button_queue = xQueueCreate( 50, sizeof( button_id_enum ) );

  if( button_queue == NULL )
  {
    ESP_LOGE(TAG, "xQueueCreate() failed. button queue wasn't created. aborting as this is a fatal error.");
    abort();
  }
}

QueueHandle_t get_button_queue(void) {
  return button_queue;
}

void buttonTask( void *pvParameters )
{

  buttons_init();

  while (1) {

    ESP_LOGI(TAG, "hi from task loop");

    vTaskDelay( pdMS_TO_TICKS(1000) );

  }

}