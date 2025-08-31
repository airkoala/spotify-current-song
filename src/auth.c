#include "auth.h"
#include "logging.h"
#include "util.h"
#include <curl/curl.h>
#include <curl/easy.h>
#include <jansson.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define API_ROOT "https://accounts.spotify.com/api"

SpotifyCredentials *refresh_token(SpotifyCredentials *creds) {
  // Refresh the access token using the refresh token

  CURL *curl;

  curl = curl_easy_init();
  if (curl == NULL) {
    errprintf("Error initializing CURL\n");
    return NULL;
  }

  curl_easy_setopt(curl, CURLOPT_URL, API_ROOT "/token");

  char postfields[500];
  sprintf(postfields, "grant_type=refresh_token&refresh_token=%s",
          creds->user_refresh_token);

  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postfields);

  struct curl_slist *headers = NULL;
  headers = curl_slist_append(
      headers, "Content-Type: application/x-www-form-urlencoded");

  // Header requires client_id:client_secret pair to be b64 encoded
  char auth_pair[100];
  sprintf(auth_pair, "%s:%s", creds->client_id, creds->client_secret);

  char *b64_auth_pair = b64enc((uint8_t *)auth_pair, strnlen(auth_pair, 100));

  char auth_header[200];
  sprintf(auth_header, "Authorization: Basic %s", b64_auth_pair);
  headers = curl_slist_append(headers, auth_header);

  dbgprintf("postfields: %s, auth_pair: %s, b64_auth_pair: %s\n", postfields,
            auth_pair, b64_auth_pair);
  dbgprintf("auth_header: %s\n", auth_header);
  free(b64_auth_pair);

  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

  // Initialise a MemoryStruct to store result into.
  MemoryStruct res_body;
  res_body.memory = malloc(1);
  res_body.size = 0;

  // Set callback to write to res_body
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_mem_callback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&res_body);

  // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

  CURLcode res = curl_easy_perform(curl);
  if (res != CURLE_OK) {
    errprintf("Error performing CURL: %s\n", curl_easy_strerror(res));
    return NULL;
  }

  long response_code;
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

  dbgprintf("Response code: %ld\n", response_code);
  dbgprintf("Received %zu bytes: %s\n", res_body.size, res_body.memory);

  // {
  //   "access_token": "...",
  //   "token_type": "Bearer",
  //   "expires_in": 3600,
  //   "scope": "...",
  //   "refresh_token": "..." // (optional)
  // }

  if (response_code != 200) {
    errprintf("Could not refresh access token. Received %ld", response_code);
    return NULL;
  }

  json_error_t error;
  json_t *root = json_loads(res_body.memory, 0, &error);
  free(res_body.memory);

  if (!root) {
    errprintf("Error decoding JSON response on line %d: %s\n", error.line,
              error.text);
    return NULL;
  }

  json_t *access_token = json_object_get(root, "access_token");
  if (!json_is_string(access_token)) {
    errprintf("Failed to get access token.\n");
    json_decref(root);
    return NULL;
  }

  json_t *expires_in = json_object_get(root, "expires_in");
  if (!json_is_integer(expires_in)) {
    errprintf("Failed to get expires_in.\n");
    json_decref(root);
    return NULL;
  }

  creds->access_token.token = strdup(json_string_value(access_token));
  creds->access_token.expires_at = time(NULL) + json_integer_value(expires_in);
  dbgprintf(
      "creds->access_token.token: %s, creds->access_token.expires_at: %u\n",
      creds->access_token.token, creds->access_token.expires_at);

  json_t *refresh_token = json_object_get(root, "refresh_token");
  if (json_is_string(refresh_token)) {
    dbgprintf("New refresh token issued. Updating.\n");
    creds->user_refresh_token = strdup(json_string_value(refresh_token));
  }

  json_decref(root);
  curl_slist_free_all(headers);
  curl_easy_cleanup(curl);

  return creds;
}

char *get_or_refresh_access_token(SpotifyCredentials *creds) {
  if (time(NULL) >= creds->access_token.expires_at) {
    SpotifyCredentials *res = refresh_token(creds);
    if (res == NULL) {
      errprintf("Failed to refresh token.\n");
      return NULL;
    }
  }

  return creds->access_token.token;
}
