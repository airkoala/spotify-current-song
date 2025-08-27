#include "config.h"
#include "logging.h"
#include "spotify.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

typedef struct SpotifyStatus {
  char *song_name;
  char *artist_name;
  uint32_t position;
  uint32_t duration;
} SpotifyStatus;

int main(int argc, char *argv[]) {
  Config *config = load_config("spotify-current-song");

  dbgprintf("Loaded config:\n")
  dbgprintf("Client ID: %s\n", config->spotify_creds.client_id);
  dbgprintf("Client Secret: %s\n", config->spotify_creds.client_secret);
  dbgprintf("Redirect URI: %s\n", config->spotify_creds.redirect_uri);
  dbgprintf("User Refresh Token: %s\n", config->spotify_creds.user_refresh_token);
  printf("\n");

  SpotifyCredentials *res = refresh_token(&config->spotify_creds);
  if (res == NULL) {
    errprintf("Failed to refresh token.\n");
    return EXIT_FAILURE;
  }

  dbgprintf("Refreshed token: %s\n", res->access_token.token);

  return EXIT_SUCCESS;
}
