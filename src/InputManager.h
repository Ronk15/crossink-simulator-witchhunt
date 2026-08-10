#pragma once
class InputManager {
public:
  // Pin del boton de encendido en el X4; solo se usa para trazas de arranque.
  static constexpr uint8_t POWER_BUTTON_PIN = 9;

public:
  InputManager() {}
  void begin() {}
  void update() {}
  bool isPressed(int button) { return false; }
  int getHeldTime(int button) { return 0; }
};
