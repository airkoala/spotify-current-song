#include "util.h"
#include "logging.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

size_t b64_encoded_size(size_t inlen) {
  size_t ret;

  ret = inlen;
  if (inlen % 3 != 0)
    ret += 3 - (inlen % 3);
  ret /= 3;
  ret *= 4;

  return ret;
}

const char b64chars[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

char *b64enc(const uint8_t *in, size_t len) {
  char *out;
  size_t elen;
  size_t i;
  size_t j;
  size_t v;

  if (in == NULL || len == 0)
    return NULL;

  elen = b64_encoded_size(len);
  out = malloc(elen + 1);
  out[elen] = '\0';

  for (i = 0, j = 0; i < len; i += 3, j += 4) {
    v = in[i];
    v = i + 1 < len ? v << 8 | in[i + 1] : v << 8;
    v = i + 2 < len ? v << 8 | in[i + 2] : v << 8;

    out[j] = b64chars[(v >> 18) & 0x3F];
    out[j + 1] = b64chars[(v >> 12) & 0x3F];
    if (i + 1 < len) {
      out[j + 2] = b64chars[(v >> 6) & 0x3F];
    } else {
      out[j + 2] = '=';
    }
    if (i + 2 < len) {
      out[j + 3] = b64chars[v & 0x3F];
    } else {
      out[j + 3] = '=';
    }
  }

  return out;
}

void ms_to_timestamp(uint32_t ms, char *dst) {
  uint32_t mins = ms / 1000 / 60;
  uint32_t secs = (ms / 1000) % 60;

  sprintf(dst, "%u:%02u", mins, secs);
}

size_t push_n_graphemes(char *dst, const char *src, size_t n) {
  size_t i;
  for (i = 0; i < n; i++) {
    if (src[i] == '\0') {
      break;
    }

    if (dst) {
      *dst = src[i];
      dst++;
    }
  }

  if (dst) {
    *dst = '\0';
  }

  return i;
}
