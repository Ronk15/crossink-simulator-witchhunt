#pragma once

#include <Arduino.h>
#include <ctime>
#include <cstddef>
#include <cstdint>

#define CROSSPOINT_SIMULATOR_HAS_DATE_FORMAT 1
#define CROSSPOINT_SIMULATOR_HAS_DATE_SEPARATOR 1

class HalClock;
extern HalClock halClock;

class HalClock {
  bool _available = false;

public:
  bool getModifyDateTime(uint16_t*, uint16_t*) const { return false; }
  enum DateFormat : uint8_t {
    MONTH_DAY_YEAR_LONG = 0,
    DAY_MONTH_YEAR_LONG = 1,
    MONTH_DAY_YEAR_NUMERIC = 2,
    DAY_MONTH_YEAR_NUMERIC = 3,
    YEAR_MONTH_DAY_NUMERIC = 4,
    MONTH_DAY_NUMERIC = 5,
    DAY_MONTH_NUMERIC = 6,
    MONTH_DAY_LONG = 7,
    DAY_MONTH_LONG = 8,
    DATE_FORMAT_COUNT
  };

  void begin();
  bool isAvailable() const { return _available; }

  // Stubs para firmware derivado de Witchhunt: en escritorio la hora del
  // sistema siempre está disponible, no hay RTC que sincronizar.
  static bool isSynced() { return true; }
  static uint64_t now() { return static_cast<uint64_t>(::time(nullptr)); }
  static uint64_t lastSyncTime() { return static_cast<uint64_t>(::time(nullptr)); }
  static bool syncNtp(const char* = nullptr) { return true; }
  static bool syncNtp(char* errBuf, size_t errLen, const char* = nullptr) {
    if (errBuf && errLen) errBuf[0] = 0;
    return true;
  }
  static void applyTimezone(int) {}
  static void wifiOff(bool = false) {}
  static void restore() {}
  static void saveBeforeSleep(bool = false) {}
  static void updatePeriodic() {}
  // Version estatica que espera Witchhunt (alli HalClock es un namespace).
  static void formatTime(char* buf, size_t bufSize, bool use24h) {
    if (!buf || !bufSize) return;
    const time_t t = ::time(nullptr);
    struct tm lt;
    localtime_r(&t, &lt);
    strftime(buf, bufSize, use24h ? "%H:%M" : "%I:%M %p", &lt);
  }

  bool getTime(uint8_t &hour, uint8_t &minute) const;
  bool getDateTime(uint16_t &year, uint8_t &month, uint8_t &day, uint8_t &hour,
                   uint8_t &minute) const;
  bool formatTime(char *buf, size_t bufSize,
                  uint8_t utcOffsetQuarterHoursBiased = 48,
                  bool use12Hour = false) const;
  bool formatDate(char *buf, size_t bufSize,
                  uint8_t utcOffsetQuarterHoursBiased = 48,
                  DateFormat dateFormat = MONTH_DAY_YEAR_LONG,
                  char numericSeparator = '/') const;
  bool syncFromNTP();
};
