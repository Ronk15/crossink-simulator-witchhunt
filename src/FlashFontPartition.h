#pragma once
// Shim de FlashFontPartition para firmware derivado de Witchhunt.
// El dispositivo guarda fuentes en una particion de flash mapeada en memoria.
// En escritorio no existe: todo responde "vacio" y el firmware cae a las
// fuentes de la tarjeta SD, que es la ruta que si queremos probar aqui.
#include <cstddef>
#include <cstdint>

namespace FlashFontPartition {

static constexpr uint8_t MAX_ENTRIES = 16;
static constexpr size_t ENTRY_SIZE = 48;
static constexpr size_t HEADER_BYTES = 8 + MAX_ENTRIES * ENTRY_SIZE;

struct Entry {
  char familyName[32];
  uint8_t pointSize;
  uint8_t pad[3];
  uint32_t dataOffset;
  uint32_t dataSize;
  uint32_t reserved;
};

inline bool beginWrite(const char *) { return false; }
inline bool appendFile(const char *, const char *, uint8_t) { return false; }
inline bool finaliseWrite() { return false; }
inline bool mmap(const char *, uint8_t, const uint8_t **outPtr, size_t *outSize) {
  if (outPtr) *outPtr = nullptr;
  if (outSize) *outSize = 0;
  return false;
}
inline void unmap() {}
inline bool hasValidIndex() { return false; }
inline bool hasEntry(const char *, uint8_t) { return false; }
inline bool hasFamilyComplete(const char *, const uint8_t *, uint8_t) { return false; }
inline bool isMapped() { return false; }

} // namespace FlashFontPartition
