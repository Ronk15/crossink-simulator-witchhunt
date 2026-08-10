#pragma once
// Shim de wolfcrypt/coding para firmware derivado de Witchhunt.
// Solo se usa Base64_Decode; se implementa a mano porque wolfSSL no entra
// en la compilacion nativa (WOLFSSL_SP_RISCV32 no compila en x86).
#include <cstdint>
#include <cstring>

typedef unsigned char byte;
typedef uint32_t word32;

static inline int Base64_Decode(const byte *in, word32 inLen, byte *out,
                                word32 *outLen) {
  static const char *T =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  if (!in || !out || !outLen) return -1;
  word32 written = 0;
  int acc = 0, bits = 0;
  for (word32 i = 0; i < inLen; ++i) {
    const char c = (char)in[i];
    if (c == '=' ) break;
    if (c == '\n' || c == '\r' || c == ' ') continue;
    const char *p = strchr(T, c);
    if (!p) return -1;
    acc = (acc << 6) | (int)(p - T);
    bits += 6;
    if (bits >= 8) {
      bits -= 8;
      if (written >= *outLen) return -1;
      out[written++] = (byte)((acc >> bits) & 0xFF);
    }
  }
  *outLen = written;
  return 0;
}
