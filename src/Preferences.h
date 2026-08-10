#pragma once
// Shim de Preferences (NVS) para firmware derivado de Witchhunt.
// El firmware guarda sus ajustes reales en la SD; esto solo cubre el
// puñado de claves que van a NVS. Persistencia en memoria, por sesión.
#include <Arduino.h>
#include <map>
#include <string>

class Preferences {
  std::map<std::string, std::string> _kv;
  bool _open = false;

public:
  bool begin(const char*, bool = false) { _open = true; return true; }
  void end() { _open = false; }
  bool clear() { _kv.clear(); return true; }
  bool remove(const char* k) { return _kv.erase(k) > 0; }
  bool isKey(const char* k) const { return _kv.count(k) > 0; }

  size_t putBool(const char* k, bool v) { _kv[k] = v ? "1" : "0"; return 1; }
  bool getBool(const char* k, bool d = false) const {
    auto it = _kv.find(k); return it == _kv.end() ? d : it->second == "1";
  }

  size_t putUChar(const char* k, uint8_t v) { _kv[k] = std::to_string(v); return 1; }
  uint8_t getUChar(const char* k, uint8_t d = 0) const {
    auto it = _kv.find(k); return it == _kv.end() ? d : (uint8_t)std::stoul(it->second);
  }

  size_t putUInt(const char* k, uint32_t v) { _kv[k] = std::to_string(v); return 4; }
  uint32_t getUInt(const char* k, uint32_t d = 0) const {
    auto it = _kv.find(k); return it == _kv.end() ? d : (uint32_t)std::stoul(it->second);
  }

  size_t putInt(const char* k, int32_t v) { _kv[k] = std::to_string(v); return 4; }
  int32_t getInt(const char* k, int32_t d = 0) const {
    auto it = _kv.find(k); return it == _kv.end() ? d : (int32_t)std::stol(it->second);
  }

  size_t putULong64(const char* k, uint64_t v) { _kv[k] = std::to_string(v); return 8; }
  uint64_t getULong64(const char* k, uint64_t d = 0) const {
    auto it = _kv.find(k); return it == _kv.end() ? d : (uint64_t)std::stoull(it->second);
  }

  size_t putString(const char* k, const char* v) { _kv[k] = v ? v : ""; return _kv[k].size(); }
  size_t putString(const char* k, const String& v) { return putString(k, v.c_str()); }
  String getString(const char* k, const String& d = String()) const {
    auto it = _kv.find(k); return it == _kv.end() ? d : String(it->second.c_str());
  }
};
