#include "config.h"
#include "logging.h"
#include "playback_status.h"
#include "auth.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {
  Config *config = load_config("spotify-current-song");

  dbgprintf("Loaded config:\n")
  dbgprintf("Client ID: %s\n", config->spotify_creds.client_id);
  dbgprintf("Client Secret: %s\n", config->spotify_creds.client_secret);
  dbgprintf("Redirect URI: %s\n", config->spotify_creds.redirect_uri);
  dbgprintf("User Refresh Token: %s\n", config->spotify_creds.user_refresh_token);
  printf("\n");

  // SpotifyCredentials *res = refresh_token(&config->spotify_creds);
  // if (res == NULL) {
  //   errprintf("Failed to refresh token.\n");
  //   return EXIT_FAILURE;
  // }

  // dbgprintf("Access token: %s\n\n", res->access_token.token);

  PlaybackStatus pbs = fetch_playback_status(&config->spotify_creds);

  dbgprintf("Playback Status:\n");
  dbgprintf("Song Name: %s\n", pbs.song_name);
  dbgprintf("Artist Name: %s\n", pbs.artist_name);
  dbgprintf("Progress: %u\n", pbs.progress);
  dbgprintf("Duration: %u\n", pbs.duration);
  dbgprintf("Updated At: %u\n", pbs.updated_at);
  dbgprintf("Status: %d\n", pbs.status);


  return EXIT_SUCCESS;
}
