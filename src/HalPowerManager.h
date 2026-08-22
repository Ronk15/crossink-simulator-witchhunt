#pragma once

#include <Arduino.h>
#include <InputManager.h>
#include <Logging.h>
#include <freertos/semphr.h>

#include <cassert>

#include "HalGPIO.h"

class HalPowerManager;
extern HalPowerManager powerManager; // Singleton

class HalPowerManager {
  int normalFreq = 0; // MHz
  bool isLowPower = false;

  enum LockMode { None, NormalSpeed };
  LockMode currentLockMode = None;
  SemaphoreHandle_t modeMutex = nullptr; // Protect access to currentLockMode

public:
  static constexpr int LOW_POWER_FREQ = 10;                   // MHz
  static constexpr unsigned long IDLE_POWER_SAVING_MS = 3000; // ms

  // Escalonado de reposo del firmware. Los valores se replican del HAL real
  // porque main.cpp los lee directamente para decidir la rama de reposo.
  static constexpr unsigned long IDLE_DOWNCLOCK_MS = 500;
  static constexpr unsigned long IDLE_LIGHT_SLEEP_MS = 1000;

  // Contadores de sueño ligero que muestra Información del sistema. En
  // escritorio solo se mueve `attempts`: la rama de reposo se alcanza igual que
  // en el dispositivo, pero nunca se duerme.
  struct LightSleepStats {
    uint32_t attempts = 0;
    uint32_t slept = 0;
    uint32_t sleptMs = 0;
    uint32_t awakeMs = 0;
    uint32_t wakeTimer = 0;
    uint32_t wakeGpio = 0;
    uint32_t rejLock = 0;
    uint32_t rejWifi = 0;
    uint32_t rejUsb = 0;
    uint32_t rejFrontlight = 0;
    uint32_t rejDebounce = 0;
    uint32_t rejIdf = 0;
  };

private:
  LightSleepStats lightSleepStats_;

public:
  // SIEMPRE declina. No hay sueño ligero en escritorio, y el simulador depende
  // del delay() de reserva del llamante para seguir cediendo el hilo: si esto
  // devolviera true, el bucle principal giraría sin ceder y la ventana SDL
  // dejaría de responder a las teclas.
  bool lightSleep(const HalGPIO & /*gpio*/) {
    lightSleepStats_.attempts++;
    return false;
  }

  const LightSleepStats &lightSleepStats() const { return lightSleepStats_; }

  void begin();

  // Control CPU frequency for power saving
  void setPowerSaving(bool enabled);

  // En el C3 la radio necesita >=80 MHz, y esto limpia a la vez isLowPower y el
  // flag de espera de forma de onda antes de subir la frecuencia. En escritorio
  // no hay frecuencia que subir; se deja el flag coherente por si algo lo lee.
  void ensureFullSpeedForRadio() { isLowPower = false; }

  // Setup wake up GPIO and enter deep sleep
  // Should be called inside main loop() to handle the currentLockMode
  void startDeepSleep(HalGPIO &gpio, bool keepLpAlive = false) const;

  // Get battery percentage (range 0-100)
  uint16_t getBatteryPercentage() const;

  // RAII helper class to manage power saving locks
  // Usage: create an instance of Lock in a scope to disable power saving, for
  // example when running a task that needs full performance. When the Lock
  // instance is destroyed (goes out of scope), power saving will be re-enabled.
  class Lock {
    friend class HalPowerManager;
    bool valid = false;

  public:
    explicit Lock();
    ~Lock();

    // Non-copyable and non-movable
    Lock(const Lock &) = delete;
    Lock &operator=(const Lock &) = delete;
    Lock(Lock &&) = delete;
    Lock &operator=(Lock &&) = delete;
  };
};
