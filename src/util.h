#pragma once

#include "logging.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
