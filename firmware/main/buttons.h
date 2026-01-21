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

#ifndef NORTHREACH_1_BUTTONS_H
#define NORTHREACH_1_BUTTONS_H


#include "freertos/FreeRTOS.h"


typedef enum {
  BTN_UP,
  BTN_DOWN,
  BTN_LEFT,
  BTN_RIGHT,
  BTN_ENTER,
  BTN_VOL_UP,
  BTN_VOL_DOWN,
  BTN_F1,
  BTN_F2,
  BTN_F3,
  BTN_F4,
} button_id_enum;

void buttonTask( void *pvParameters );

BaseType_t xButtonQueueReceive(button_id_enum *ButtonID, const TickType_t xTicksToWait);

#endif // NORTHREACH_1_BUTTONS_H
