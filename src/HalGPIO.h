#pragma once

#include <Arduino.h>
#include <BatteryMonitor.h>
#include <InputManager.h>

// Display SPI pins (custom pins for XteinkX4, not hardware SPI defaults)
#ifndef EPD_SCLK
#define EPD_SCLK 8 // SPI Clock
#endif
#ifndef EPD_MOSI
#define EPD_MOSI 10 // SPI MOSI (Master Out Slave In)
#endif
#ifndef EPD_CS
#define EPD_CS 21 // Chip Select
#endif
#ifndef EPD_DC
#define EPD_DC 4 // Data/Command
#endif
#ifndef EPD_RST
#define EPD_RST 5 // Reset
#endif
#ifndef EPD_BUSY
#define EPD_BUSY 6 // Busy
#endif

#define SPI_MISO                                                               \
  7 // SPI MISO, shared between SD card and display (Master In Slave Out)

#define BAT_GPIO0 0 // Battery voltage

#define UART0_RXD 20 // Used for USB connection detection

class HalGPIO {
#if CROSSPOINT_EMULATED == 0
  InputManager inputMgr;
#endif

public:
  enum class DeviceType : uint8_t { X4, X3 };

private:
  DeviceType _deviceType = DeviceType::X4;

public:
  HalGPIO() = default;

  // Inline device type helpers for cleaner downstream checks
  inline bool deviceIsX3() const { return _deviceType == DeviceType::X3; }
  inline bool deviceIsX4() const { return _deviceType == DeviceType::X4; }
  bool isXteinkDevice() const;
  bool hasEdgeSideButtons() const;

  // Start button GPIO and setup SPI for screen and SD card
  void begin();

  // Clears the per-frame press/release edge latches. Must be called exactly
  // once per frame (before the firmware's loop()), NOT on every update().
  void beginFrame();

  // Button input methods
  void update();
  bool isPressed(uint8_t buttonIndex) const;
  bool wasPressed(uint8_t buttonIndex) const;
  bool wasAnyPressed() const;
  bool wasReleased(uint8_t buttonIndex) const;
  bool wasAnyReleased() const;
  unsigned long getHeldTime() const;
  unsigned long getPowerButtonHeldTime() const;
  bool hasTouch() const;
  bool hasHomeKey() const;
  bool wasHomeKeyPressed() const;
  bool wasHomeKeyTapped() const;
  bool wasHomeKeyLongPressed() const;
  bool wasTouchTap(float &nx, float &ny) const;
  bool wasTouchDown(float &nx, float &ny) const;
  bool wasTouchReleased() const;
  bool isTouchTapCandidate(float &nx, float &ny, unsigned long &heldMs) const;
  bool isTouchHeldAt(float &nx, float &ny) const;
  unsigned long lastTouchHeldMs() const;
  bool wasSwipe(float &nxStart, float &nyStart, float &nxEnd,
                float &nyEnd) const;
  bool wasTouchActivity() const;
  void setSharedConfirmPowerShortPressEmitsPower(bool enabled);
  bool consumeSimulatorSleepRequest();

  // Setup wake up GPIO and enter deep sleep
  void startDeepSleep();

  // Simulated power-button wakes are always accepted so host boot can continue.

  // Check if USB is connected
  bool isUsbConnected() const;

  // Returns true once per edge (plug or unplug) since the last update()
  bool wasUsbStateChanged() const;

  enum class WakeupReason { PowerButton, AfterFlash, AfterUSBPower, Other };

  WakeupReason getWakeupReason() const;

// --- Cola de flancos para firmware derivado de Witchhunt ---
  // El shim del simulador entrega entrada por sondeo (wasPressed/beginFrame).
  // Witchhunt agrega encima una cola FIFO que llena un muestreador en segundo
  // plano. En escritorio no existe ese muestreador, así que la cola va siempre
  // vacía y ButtonEventManager cae a su ruta por sondeo.
  struct ButtonEdge {
    uint8_t button;
    bool pressed;
    unsigned long timeMs;
  };

  bool hasPendingInput() const { return wasAnyPressed(); }
  bool popButtonEdge(ButtonEdge &out) const;
  void flushButtonEdges() const;
  // El escritorio no tiene muestreador en segundo plano ni botones fisicos.
  bool isHeldNow(uint8_t) const { return false; }
  void waitForStablePowerRelease() {}
  void startInputSampler() {}
  void stopInputSampler() {}
  unsigned samplerStackHighWater() const { return 0; }

  // Gestos de despertado aceptados, para firmware derivado de Witchhunt.
  struct WakeGestures {
    bool shortAllowed = false;
    bool doubleClick = false;
    bool longHold = true;
  };

  // Veredicto del guardia de despertado, para firmware derivado de Witchhunt.
  // El simulador acepta siempre el despertado sintetico, asi que el veredicto
  // es LongHold y accepted() responde true.
  enum class WakeVerdict : uint8_t {
    NotPressed,
    ShortPress,
    LongHold,
    DoubleClick,
    ReleasedEarly,
    NoSecondPress,
  };
  struct WakeCheck {
    WakeVerdict verdict = WakeVerdict::LongHold;
    uint16_t decidedAtMs = 0;
    uint16_t heldMs = 0;
    bool accepted() const {
      return verdict == WakeVerdict::ShortPress || verdict == WakeVerdict::LongHold ||
             verdict == WakeVerdict::DoubleClick;
    }
  };
  static const char* wakeVerdictName(WakeVerdict v) {
    switch (v) {
      case WakeVerdict::NotPressed: return "NotPressed";
      case WakeVerdict::ShortPress: return "ShortPress";
      case WakeVerdict::LongHold: return "LongHold";
      case WakeVerdict::DoubleClick: return "DoubleClick";
      case WakeVerdict::ReleasedEarly: return "ReleasedEarly";
      case WakeVerdict::NoSecondPress: return "NoSecondPress";
    }
    return "Unknown";
  }

  // Firma de Witchhunt: el simulador acepta siempre el despertado sintetico.
  WakeCheck verifyPowerButtonWakeup(WakeGestures, uint16_t) {
    WakeCheck c;
    c.verdict = WakeVerdict::LongHold;
    return c;
  }

  // Button indices
  static constexpr uint8_t BTN_BACK = 0;
  static constexpr uint8_t BTN_CONFIRM = 1;
  static constexpr uint8_t BTN_LEFT = 2;
  static constexpr uint8_t BTN_RIGHT = 3;
  static constexpr uint8_t BTN_UP = 4;
  static constexpr uint8_t BTN_DOWN = 5;
  static constexpr uint8_t BTN_POWER = 6;
};

extern HalGPIO gpio; // Singleton
