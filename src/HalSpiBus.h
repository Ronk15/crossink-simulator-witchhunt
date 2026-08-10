#pragma once
// Shim de HalSpiBus para firmware derivado de Witchhunt.
// En el dispositivo serializa pantalla y SD sobre el bus SPI compartido.
// En escritorio no hay bus, asi que Lock es un objeto vacio.
class HalSpiBus {
public:
  class Lock {
  public:
    Lock() {}
    ~Lock() {}
    Lock(const Lock &) = delete;
    Lock &operator=(const Lock &) = delete;
  };

  static HalSpiBus &getInstance() {
    static HalSpiBus instance;
    return instance;
  }
  static void begin() {}

private:
  HalSpiBus() {}
};
