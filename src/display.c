#include "display.h"
#include "playback_status.h"
#include "util.h"
#include <stddef.h>
#include <string.h>
#include <time.h>
#include <uchar.h>

// TODO: Move to config
const char FORMAT[] = " [sp/d] c";
const char SCROLLER_FORMAT[] = "a - t";

#define SCROLLER_STEP_DELAY_MS 500
#define MAX_LENGTH 60
#define PAUSE_ICON "󰏥 "
// #define ELLIPSIS "…"
#define ELLIPSIS "-"

void build_output(char *dst, const PlaybackStatus *pbs) {
  if (pbs->status == STOPPED) {
    return;
  }

  for (size_t i = 0; FORMAT[i] != '\0'; i++) {
    char buf[10];
    switch (FORMAT[i]) {
    case 's':
      if (pbs->status == PAUSED) {
        dst = stpcpy(dst, PAUSE_ICON);
      }
      break;
    case 'p':
      // Ensure that it shows current progress and not progress at last update.
      ms_to_timestamp(pbs->progress + (time(NULL) - pbs->updated_at), buf);
      dst = stpcpy(dst, buf);
      break;
    case 'd':
      ms_to_timestamp(pbs->duration, buf);
      dst = stpcpy(dst, buf);
      break;
    case 'c':
      dst = build_scroller(dst, pbs, MAX_LENGTH - strlen(dst), SCROLLER_STEP_DELAY_MS);
      break;
    default:
      *dst = FORMAT[i];
      dst++;
      *dst = '\0';
    }
  }
}

char *build_scroller(char *dst, const PlaybackStatus *pbs, uint32_t scroller_window_length,
                     uint32_t step_delay_ms) {
  // Intentionally oversized.
  char scroller[1000];
  char *bufptr = scroller;
  for (size_t i = 0; SCROLLER_FORMAT[i] != '\0'; i++) {
    switch (SCROLLER_FORMAT[i]) {
    case 'a':
      bufptr = stpcpy(bufptr, pbs->artist_name);
      break;
    case 't':
      bufptr = stpcpy(bufptr, pbs->song_name);
      break;
    default:
      *bufptr = SCROLLER_FORMAT[i];
      bufptr++;
      *bufptr = '\0';
    }
  }

  // TODO: convert to use graphemes
  size_t full_scroller_len = strlen(scroller);
  dbgprintf("full_scroller_len: %zu, full_scroller: %s\n", full_scroller_len,
         scroller);

  const size_t max_scroll_pos = (scroller_window_length <= full_scroller_len)
                                ? (full_scroller_len - scroller_window_length + 1)
                                : 1;

  // in graphemes
  size_t start_pos = (pbs->progress / step_delay_ms) % max_scroll_pos;
  char *scrollerptr = &scroller[push_n_graphemes(NULL, scroller, start_pos)];

  if (start_pos > 0) {
    // Push ELLIPSIS and skip first char in scroller
    dst += push_n_graphemes(dst, ELLIPSIS, 1);
    scrollerptr++;
    scroller_window_length--;
  }

  dbgprintf("start_pos: %zu, max_scroll_pos: %zu\n", start_pos, max_scroll_pos);

  size_t n_bytes = push_n_graphemes(dst, scrollerptr, scroller_window_length - 1);
  dst += n_bytes;
  scrollerptr += n_bytes;

  // check if pushing another grapheme cluster will result in exhausting the scroller or not
  n_bytes = push_n_graphemes(NULL, scrollerptr, 1);
  if (*(scrollerptr + n_bytes) == '\0') {
    // if no more graphemes after this one then push this
    dst += push_n_graphemes(dst, scrollerptr, 1);
  } else {
    // else, push ELLIPSIS
    dst += push_n_graphemes(dst, ELLIPSIS, 1);
  }

  return dst;
}

// char *cat_names(char *dst, const PlaybackStatus *pbs) {
//
// }
