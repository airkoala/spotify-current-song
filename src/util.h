#pragma once

#include "logging.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))

// Base64 encode input data
char *b64enc(const uint8_t *in, size_t len);

typedef struct MemoryStruct {
  char *memory;
  size_t size;
} MemoryStruct;
 
static size_t write_mem_callback(void *contents, size_t size, size_t nmemb,
                                 void *userp) {
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;

  char *ptr = (char *)realloc(mem->memory, mem->size + realsize + 1);
  if (!ptr) {
    /* out of memory! */
    errprintf("not enough memory (realloc returned NULL)\n");
    return 0;
  }

  mem->memory = ptr;
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;

  return realsize;
}

void ms_to_timestamp(uint32_t ms, char *dst);

// Returns: number of bytes pushed - 1 (null terminator)
size_t push_n_graphemes(char *dst, const char *src, size_t n);
size_t count_graphemes(const char *str);
