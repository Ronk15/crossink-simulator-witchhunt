#pragma once

#include "esp_err.h"
#include "esp_partition.h"

inline const esp_partition_t *esp_ota_get_running_partition() {
  static esp_partition_t p{};
  return &p;
}
inline const esp_partition_t *esp_ota_get_next_update_partition(const esp_partition_t *) {
  return nullptr;
}

inline esp_err_t esp_ota_mark_app_valid_cancel_rollback() {
  return ESP_OK;
}

// Estado de particion OTA para firmware derivado de Witchhunt.
// En escritorio no hay arranque tentativo que verificar.
typedef enum {
  ESP_OTA_IMG_NEW = 0,
  ESP_OTA_IMG_PENDING_VERIFY = 1,
  ESP_OTA_IMG_VALID = 2,
  ESP_OTA_IMG_INVALID = 3,
  ESP_OTA_IMG_ABORTED = 4,
  ESP_OTA_IMG_UNDEFINED = 0xFF,
} esp_ota_img_states_t;

inline int esp_ota_get_state_partition(const esp_partition_t *,
                                       esp_ota_img_states_t *out) {
  if (out) *out = ESP_OTA_IMG_VALID;
  return 0; // ESP_OK
}
