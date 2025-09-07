#pragma once

// #define DEBUG

#ifdef DEBUG
#define dbgprintf(...) \
  fprintf(stderr, "DEBUG: "); \
  fprintf(stderr, __VA_ARGS__);
#else
// Disable debug prints
#define dbgprintf(...)
#endif // DEBUG

#define errprintf(...) \
  fprintf(stderr, "ERROR: "); \
  fprintf(stderr, __VA_ARGS__);

