#pragma once

#include <stdint.h>

typedef struct SpotifyAccessToken {
  char *token;
  uint32_t expires_at;
} SpotifyAccessToken;

typedef struct SpotifyCredentials {
  char *client_id;
  char *client_secret;
  char *user_refresh_token;
  SpotifyAccessToken access_token;
  char *redirect_uri;
} SpotifyCredentials;

SpotifyCredentials *refresh_token(SpotifyCredentials *creds);
char *get_or_refresh_access_token(SpotifyCredentials *creds);
