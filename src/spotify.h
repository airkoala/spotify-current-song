#pragma once

#define API_ROOT "https://accounts.spotify.com/api"

typedef struct SpotifyCredentials {
  char *client_id;
  char *client_secret;
  char *user_refresh_token;
  char *user_access_token;
  char *redirect_uri;
} SpotifyCredentials;

int refresh_token(SpotifyCredentials  *creds);
