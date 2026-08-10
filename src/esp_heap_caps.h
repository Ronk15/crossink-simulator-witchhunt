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

// Sonda de bloques del heap (PR #124 de Witchhunt). En escritorio no hay
// un asignador que inspeccionar: se reportan cifras coherentes y estables
// para que las trazas de fragmentacion no acusen un problema inexistente.
typedef struct {
  size_t total_free_bytes;
  size_t total_allocated_bytes;
  size_t largest_free_block;
  size_t minimum_free_bytes;
  size_t allocated_blocks;
  size_t free_blocks;
  size_t total_blocks;
} multi_heap_info_t;

static inline void heap_caps_get_info(multi_heap_info_t *info, uint32_t caps) {
  (void)caps;
  if (!info) return;
  info->total_free_bytes = 4u * 1024u * 1024u;
  info->total_allocated_bytes = 1u * 1024u * 1024u;
  info->largest_free_block = 2u * 1024u * 1024u;
  info->minimum_free_bytes = 4u * 1024u * 1024u;
  info->allocated_blocks = 100;
  info->free_blocks = 4;
  info->total_blocks = 104;
}
