#pragma once
// Shim de esp_heap_caps para firmware derivado de Witchhunt.
// El escritorio tiene heap virtual: reportamos cifras generosas y fijas
// para que las rutas de "memoria insuficiente" del firmware nunca disparen.
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#define MALLOC_CAP_EXEC     (1 << 0)
#define MALLOC_CAP_32BIT    (1 << 1)
#define MALLOC_CAP_8BIT     (1 << 2)
#define MALLOC_CAP_DMA      (1 << 3)
#define MALLOC_CAP_SPIRAM   (1 << 10)
#define MALLOC_CAP_INTERNAL (1 << 11)
#define MALLOC_CAP_DEFAULT  (1 << 12)

// 4 MB libres, 2 MB de bloque contiguo mayor.
static inline size_t heap_caps_get_free_size(uint32_t caps) {
  (void)caps;
  return 4u * 1024u * 1024u;
}

static inline size_t heap_caps_get_largest_free_block(uint32_t caps) {
  (void)caps;
  return 2u * 1024u * 1024u;
}

static inline size_t heap_caps_get_minimum_free_size(uint32_t caps) {
  (void)caps;
  return 4u * 1024u * 1024u;
}

static inline size_t heap_caps_get_total_size(uint32_t caps) {
  (void)caps;
  return 8u * 1024u * 1024u;
}

static inline bool heap_caps_check_integrity_all(bool print_errors) {
  (void)print_errors;
  return true;
}

static inline bool heap_caps_check_integrity(uint32_t caps, bool print_errors) {
  (void)caps; (void)print_errors;
  return true;
}

static inline void* heap_caps_malloc(size_t size, uint32_t caps) {
  (void)caps;
  return malloc(size);
}

static inline void* heap_caps_calloc(size_t n, size_t size, uint32_t caps) {
  (void)caps;
  return calloc(n, size);
}

static inline void* heap_caps_realloc(void* p, size_t size, uint32_t caps) {
  (void)caps;
  return realloc(p, size);
}

static inline void heap_caps_free(void* p) { free(p); }

static inline void heap_caps_dump(uint32_t caps) { (void)caps; }
static inline void heap_caps_dump_all(void) {}
