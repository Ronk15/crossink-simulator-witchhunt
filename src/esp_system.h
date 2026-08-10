#pragma once
#include <cstdint>
inline void esp_restart() {}
inline uint32_t esp_get_free_heap_size() { return 1000000; }
#ifndef ESP_RANDOM_SHIM
#define ESP_RANDOM_SHIM
#include <stdlib.h>
static inline uint32_t esp_random(void) {
  return ((uint32_t)rand() << 16) ^ (uint32_t)rand();
}
#endif
