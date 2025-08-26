#include "spotify.h"
#include <curl/curl.h>
#include <stdio.h>

int refresh_token(SpotifyCredentials  *creds) {
  CURL *curl;
  CURLcode res;

  curl = curl_easy_init();
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, API_ROOT"/token");

    char postfields[100];
    sprintf(postfields, "grant_type=refresh_token&refresh_token=%s", creds->user_refresh_token);

    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postfields);

    struct curl_slist *headers = NULL;
    curl_slist_append(headers, "content-type: application/x-www-form-urlencoded");
    curl_slist_append(headers, "Authorization: Basic ");

  }
  


  return 0;

}
