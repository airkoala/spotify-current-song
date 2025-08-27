#include "spotify.h"
#include "logging.h"
#include "util.h"
#include <curl/curl.h>
#include <curl/easy.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
  headers = curl_slist_append(headers,
                    "Content-Type: application/x-www-form-urlencoded");

  // Header requires client_id:client_secret pair to be b64 encoded
  char auth_pair[100];
  sprintf(auth_pair, "%s:%s", creds->client_id, creds->client_secret);

  char *b64_auth_pair = b64enc(auth_pair, strnlen(auth_pair, 100));

  char auth_header[200];
  sprintf(auth_header, "Authorization: Basic %s", b64_auth_pair);
  headers = curl_slist_append(headers, auth_header);

  dbgprintf("postfields: %s, auth_pair: %s, b64_auth_pair: %s\n", postfields, auth_pair, b64_auth_pair);
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

  dbgprintf("Received %zu bytes: %s\n", res_body.size, res_body.memory);

  // TODO: Parse response


  free(res_body.memory);
  curl_slist_free_all(headers);
  curl_easy_cleanup(curl);

  return creds;
}
