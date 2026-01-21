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

#include "hid.h"

#include "buttons.h"
#include "esp_log.h"

#include "class/hid/hid_device.h"
#include "driver/gpio.h"
#include "tinyusb.h"
#include "tinyusb_default_config.h"

static const char *TAG = "hid_task";

/************* TinyUSB descriptors ****************/

#define TUSB_DESC_TOTAL_LEN                                                    \
  (TUD_CONFIG_DESC_LEN + CFG_TUD_HID * TUD_HID_DESC_LEN)

// HID report descriptor
const uint8_t hid_report_descriptor[] = {
    TUD_HID_REPORT_DESC_KEYBOARD(HID_REPORT_ID(HID_ITF_PROTOCOL_KEYBOARD)),
};

// string descriptor
const char *hid_string_descriptor[5] = {
    // array of pointer to string descriptors
    (char[]){0x09, 0x04}, // 0: is supported language is English (0x0409)
    "@sthivaios",         // 1: Manufacturer
    "NorthReach 1",       // 2: Product
    "01",                 // 3: Serials, should use chip ID
    "Wired whiteboard presentation controller", // 4: HID
};

/**
 * @brief Configuration descriptor
 *
 * This is a simple configuration descriptor that defines 1 configuration and 1
 * HID interface
 */
static const uint8_t hid_configuration_descriptor[] = {
    // Configuration number, interface count, string index, total length,
    // attribute, power in mA
    TUD_CONFIG_DESCRIPTOR(1, 1, 0, TUSB_DESC_TOTAL_LEN,
                          TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 100),

    // Interface number, string index, boot protocol, report descriptor len, EP
    // In address, size & polling interval
    TUD_HID_DESCRIPTOR(0, 4, false, sizeof(hid_report_descriptor), 0x81, 16,
                       10),
};

/********* TinyUSB HID callbacks ***************/

// Invoked when received GET HID REPORT DESCRIPTOR request
// Application return pointer to descriptor, whose contents must exist long
// enough for transfer to complete
uint8_t const *tud_hid_descriptor_report_cb(uint8_t instance) {
  // We use only one interface and one HID report descriptor, so we can ignore
  // parameter 'instance'
  return hid_report_descriptor;
}

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id,
                               hid_report_type_t report_type, uint8_t *buffer,
                               uint16_t reqlen) {
  (void)instance;
  (void)report_id;
  (void)report_type;
  (void)buffer;
  (void)reqlen;

  return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id,
                           hid_report_type_t report_type, uint8_t const *buffer,
                           uint16_t bufsize) {}

void tud_resume_cb(void) {
  ESP_LOGI(TAG, "=== USB RESUMED ===");

  vTaskDelay(pdMS_TO_TICKS(500)); // Wait for things to settle

  // Check USB stack state
  ESP_LOGI(TAG, "tud_mounted(): %d", tud_mounted());
  ESP_LOGI(TAG, "tud_suspended(): %d", tud_suspended());
  ESP_LOGI(TAG, "tud_ready(): %d", tud_ready());
  ESP_LOGI(TAG, "tud_hid_ready(): %d", tud_hid_ready());

  // Try sending an empty report to "wake up" the endpoint
  ESP_LOGI(TAG, "Attempting to send empty HID report...");
  bool result = tud_hid_keyboard_report(HID_ITF_PROTOCOL_KEYBOARD, 0, NULL);
  ESP_LOGI(TAG, "Send result: %d", result);

  vTaskDelay(pdMS_TO_TICKS(100));
  ESP_LOGI(TAG, "After send - tud_hid_ready(): %d", tud_hid_ready());

  if (!tud_hid_ready()) {
    tud_disconnect();
    vTaskDelay(pdMS_TO_TICKS(100));
    tud_connect();
    ESP_LOGW(TAG, "tud_hid_ready() still not true, connection was reset");
  }
}

/********* Application ***************/

int failedKeypresses = 0;

void app_send_hid_keypress(int key) {

  if (!tud_hid_ready()) {
    ESP_LOGW(TAG, "HID not ready. The last keypress will be flushed from the "
                  "queue without being sent.");
    failedKeypresses++;
    if (failedKeypresses > 3) {
      ESP_LOGE(TAG, "Over 3 failed keypresses, HID is messed up. Resetting.");
      esp_restart();
    }
    return;
  };

  uint8_t keycode[6] = {key};
  tud_hid_keyboard_report(HID_ITF_PROTOCOL_KEYBOARD, 0, keycode);
  while (!tud_hid_ready()) {
    vTaskDelay(pdMS_TO_TICKS(1));
  }
  tud_hid_keyboard_report(HID_ITF_PROTOCOL_KEYBOARD, 0, NULL);
}

void hid_task(void *pvParameters) {
  ESP_LOGI(TAG, "USB initialization");
  tinyusb_config_t tusb_cfg = TINYUSB_DEFAULT_CONFIG();

  tusb_cfg.descriptor.device = NULL;
  tusb_cfg.descriptor.full_speed_config = hid_configuration_descriptor;
  tusb_cfg.descriptor.string = hid_string_descriptor;
  tusb_cfg.descriptor.string_count =
      sizeof(hid_string_descriptor) / sizeof(hid_string_descriptor[0]);
#if (TUD_OPT_HIGH_SPEED)
  tusb_cfg.descriptor.high_speed_config = hid_configuration_descriptor;
#endif // TUD_OPT_HIGH_SPEED

  ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));
  ESP_LOGI(TAG, "USB initialization DONE");

  while (1) {
    while (tud_mounted()) {
      button_id_enum button_pressed;
      xButtonQueueReceive(&button_pressed, portMAX_DELAY);
      static const uint16_t button_to_hid[11] = {
          [BTN_LEFT] = HID_KEY_ARROW_LEFT, [BTN_RIGHT] = HID_KEY_ARROW_RIGHT,
          [BTN_UP] = HID_KEY_ARROW_UP,     [BTN_DOWN] = HID_KEY_ARROW_DOWN,
          [BTN_ENTER] = HID_KEY_ENTER,
      };
      app_send_hid_keypress(button_to_hid[button_pressed]);
    }
  }
}