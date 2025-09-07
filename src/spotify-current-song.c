#include "config.h"
#include "display.h"
#include "logging.h"
#include "playback_status.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  Config *config = load_config("spotify-current-song");

  dbgprintf("Loaded config:\n")
  dbgprintf("Client ID: %s\n", config->spotify_creds.client_id);
  dbgprintf("Client Secret: %s\n", config->spotify_creds.client_secret);
  dbgprintf("Redirect URI: %s\n", config->spotify_creds.redirect_uri);
  dbgprintf("User Refresh Token: %s\n", config->spotify_creds.user_refresh_token);
  dbgprintf("\n");

  SpotifyCredentials *creds = refresh_token(&config->spotify_creds);
  if (creds == NULL) {
    errprintf("Failed to refresh token.\n");
    return EXIT_FAILURE;
  }

  dbgprintf("Access token: %s\n\n", creds->access_token.token);

  PlaybackStatus pbs = fetch_playback_status(creds);

  // PlaybackStatus pbs = {
  //   .song_name = "Ingenting kan forhindre at små struber skælver en forårsnat",
  //   .artist_name = "Vægtløs",
  //   // .progress = 397124,
  //   .progress = 0,
  //   .duration = 584573,
  //   .updated_at = 150278575,
  //   .status = PLAYING
  // };

  dbgprintf("Playback Status:\n");
  dbgprintf("Song Name: %s\n", pbs.song_name);
  dbgprintf("Artist Name: %s\n", pbs.artist_name);
  dbgprintf("Progress: %u\n", pbs.progress);
  dbgprintf("Duration: %u\n", pbs.duration);
  dbgprintf("Updated At: %u\n", pbs.updated_at);
  dbgprintf("Status: %d\n\n", pbs.status);

  char output[500];

  while (pbs.progress < pbs.duration) {
    build_output(output, &pbs);
    puts(output);
    pbs.progress += 500;
    usleep(500 * 1000);
  }
  // printf("output: %s\n", output);

  return EXIT_SUCCESS;
}
