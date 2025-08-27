#pragma once

#define DEBUG

#ifdef DEBUG
#define dbgprintf(...) \
  fprintf(stderr, "DEBUG: "); \
  fprintf(stderr, __VA_ARGS__);
#endif // DEBUG

#define errprintf(...) \
  fprintf(stderr, "ERROR: "); \
  fprintf(stderr, __VA_ARGS__);

