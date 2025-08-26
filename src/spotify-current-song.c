#include "config.h"
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
  printf("Client ID: %s\n", config->spotify_creds.client_id);
  printf("Client Secret: %s\n", config->spotify_creds.client_secret);
  printf("Redirect URI: %s\n", config->spotify_creds.redirect_uri);
  printf("User Refresh Token: %s\n", config->spotify_creds.user_refresh_token);
  printf("\n");

  return EXIT_SUCCESS;
}
