#pragma once

#include "auth.h"
#include <stdint.h>

enum PlaybackState { PLAYING, PAUSED, STOPPED };

typedef struct PlaybackStatus {
  char song_name[1000];
  char artist_name[1000];
  uint32_t progress;
  uint32_t duration;
  uint32_t updated_at;
  enum PlaybackState status;
} PlaybackStatus;

PlaybackStatus fetch_playback_status(SpotifyCredentials *creds);
PlaybackStatus parse_pbs(const char *json);
