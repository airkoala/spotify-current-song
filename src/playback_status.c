#include "playback_status.h"
#include "auth.h"
#include "logging.h"
#include "util.h"
#include <curl/curl.h>
#include <curl/easy.h>
#include <jansson.h>
#include <pthread.h>
#include <stdio.h>

#define PLAYBACK_STATE_ENDPOINT "https://api.spotify.com/v1/me/player"

PlaybackStatus fetch_playback_status(SpotifyCredentials *creds) {
  CURL *curl;

  curl = curl_easy_init();
  if (curl == NULL) {
    errprintf("Error initializing CURL\n");
    pthread_exit(NULL);
  }

  curl_easy_setopt(curl, CURLOPT_URL, PLAYBACK_STATE_ENDPOINT);
  curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);

  char auth_header[500];
  sprintf(auth_header, "Authorization: Bearer %s",
          get_or_refresh_access_token(creds));
  struct curl_slist *headers = curl_slist_append(NULL, auth_header);

  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

  // Initialise a MemoryStruct to store result into.
  MemoryStruct res_body;
  res_body.memory = malloc(1);
  res_body.size = 0;

  // Set callback to write to res_body
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_mem_callback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&res_body);

  curl_easy_perform(curl);

  long response_code;
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

  dbgprintf("Received %ld: %s", response_code, res_body.memory);

  PlaybackStatus status;
  if (response_code == 200) {
    status = parse_pbs(res_body.memory);
  } else if (response_code == 204) {
    // 204 means playback is stopped
    status.status = STOPPED;
  } else {
    errprintf("Failed to get playback status.\n");
  }

  free(res_body.memory);
  curl_slist_free_all(headers);
  curl_easy_cleanup(curl);

  return status;
}

PlaybackStatus parse_pbs(const char *json) {
  PlaybackStatus pbs;

  // {
  //   "is_playing" : true,
  //   "timestamp" : 1756604893833,
  //   "progress_ms" : 110336,
  //   "item" : {
  //     "artists" : [ {
  //       "name" : "An Abstract Illusion",
  //     } ],
  //     "duration_ms" : 443653,
  //     "name" : "Slaves",
  //   },
  // }

  json_error_t error;
  json_t *root = json_loads(json, 0, &error);

  if (!root) {
    errprintf("Error decoding JSON response on line %d: %s\n", error.line,
              error.text);
    return pbs;
  }

  json_t *is_playing = json_object_get(root, "is_playing");
  // API responds with 204 when player stopped so we dont deal with that here
  pbs.status = json_boolean_value(is_playing) ? PLAYING : PAUSED;

  json_t *timestamp = json_object_get(root, "timestamp");
  pbs.updated_at = json_integer_value(timestamp);

  json_t *progress_ms = json_object_get(root, "progress_ms");
  pbs.progress = json_integer_value(progress_ms);

  json_t *item = json_object_get(root, "item");

  json_t *artists = json_object_get(item, "artists");
  json_t *artist = json_array_get(artists, 0);
  json_t *artist_name = json_object_get(artist, "name");
  const char *artist_name_s = json_string_value(artist_name);

  if (artist_name_s) {
    strncpy(pbs.artist_name, artist_name_s, 100);
  } else {
    dbgprintf("Couldn't parse artist name.\n");
  }

  json_t *duration = json_object_get(item, "duration_ms");
  pbs.duration = json_integer_value(duration);

  json_t *name = json_object_get(item, "name");
  const char *song_name_s = json_string_value(name);
  if (song_name_s) {
    strncpy(pbs.song_name, song_name_s, 100);
  } else {
    dbgprintf("Couldn't parse song name.\n");
  }

  json_decref(root);
  return pbs;
}
