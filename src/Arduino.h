#pragma once
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstdarg>
#include <cstdint>
#include <string>
#include <thread>

#define PROGMEM
#define ICACHE_RODATA_ATTR
#define IRAM_ATTR
#define DRAM_ATTR
#define RTC_NOINIT_ATTR
#define PGM_P const char *
#define PSTR(s) (s)

inline unsigned long millis() {
  using namespace std::chrono;
  static const auto start = steady_clock::now();
  return duration_cast<milliseconds>(steady_clock::now() - start).count();
}

inline unsigned long micros() {
  using namespace std::chrono;
  static const auto start = steady_clock::now();
  return duration_cast<microseconds>(steady_clock::now() - start).count();
}

inline void delay(unsigned long ms) {
  std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}
inline void yield() { std::this_thread::yield(); }

#include "HardwareSerial.h"
#include "Print.h"
#include "WString.h"

struct ESPMock {
  uint32_t getFreeHeap() { return 1024 * 1024; }
  void restart();
  uint32_t getHeapSize() { return 1024 * 1024; }
  uint32_t getMinFreeHeap() { return 1024 * 1024; }
  uint32_t getMaxAllocHeap() { return 1024 * 1024; }
  const char *getChipModel() { return "ESP32-C3-SIM"; }
  uint8_t getChipRevision() { return 4; }
  uint32_t getFlashChipSize() { return 16u * 1024 * 1024; }
};
extern ESPMock ESP;

inline long random(long max) { return std::rand() % max; }

template <typename A, typename B>
constexpr auto max(A a, B b) -> decltype(a > b ? a : b) {
  return a > b ? a : b;
}
template <typename A, typename B>
constexpr auto min(A a, B b) -> decltype(a < b ? a : b) {
  return a < b ? a : b;
}

// Stub para firmware derivado de Witchhunt: frecuencia nominal en escritorio.
static inline uint32_t getCpuFrequencyMhz() { return 160; }

// GPIO crudo para firmware derivado de Witchhunt.
// En escritorio no hay pines: el boton de encendido se lee siempre suelto (HIGH).
#ifndef LOW
#define LOW 0
#endif
#ifndef HIGH
#define HIGH 1
#endif
#ifndef INPUT
#define INPUT 0
#define OUTPUT 1
#define INPUT_PULLUP 2
#endif
static inline int digitalRead(uint8_t) { return HIGH; }
static inline void digitalWrite(uint8_t, int) {}
static inline void pinMode(uint8_t, int) {}

// Arranque y sueño profundo, para firmware derivado de Witchhunt.
static inline void gpio_deep_sleep_hold_dis(void) {}
static inline void gpio_deep_sleep_hold_en(void) {}
static inline uint32_t esp_get_minimum_free_heap_size(void) {
  return 1024u * 1024u;
}

// Trae esp_restart, esp_get_free_heap_size y esp_random a todo el firmware.
#include "esp_system.h"
