#include "config.h"
#include "logging.h"
#include "auth.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Config *new_config(const char *bin_name) {
  Config *conf = malloc(sizeof(Config));

  conf->bin_name = strndup(bin_name, 100);

  conf->spotify_creds.client_id = NULL;
  conf->spotify_creds.client_secret = NULL;
  conf->spotify_creds.access_token.token = NULL;
  conf->spotify_creds.access_token.expires_at = 0;
  conf->spotify_creds.user_refresh_token = NULL;
  conf->spotify_creds.redirect_uri = NULL;

  return conf;
}

Config *read_config_from_file(const char *path, const char *bin_name) {
  FILE *fptr = fopen(path, "r");

  if (fptr == NULL) {
    dbgprintf("Failed to open %s. errno: %d\n", path, errno);
    return NULL;
  }

  Config *config = new_config(bin_name);

  // line buffer, oversized
  char line[2000];

  while (fgets(line, (sizeof line / sizeof line[0]), fptr)) {
    char key[50];
    char val[500];
    int res = sscanf(line, "%s = %s", key, val);

    // printf("line: %s, res: %d, key: %s, val: %s\n", line, res, key, val);

    if (res != 2) {
      fclose(fptr);
      dbgprintf("Failed to read config.\n");
      return NULL;
    }

    if (strncmp(key, "SPOTIFY_CLIENT_ID", 50) == 0) {
      config->spotify_creds.client_id = strndup(val, 500);
    } else if (strncmp(key, "SPOTIFY_CLIENT_SECRET", 50) == 0) {
      config->spotify_creds.client_secret = strndup(val, 500);
    } else if (strncmp(key, "SPOTIFY_REDIRECT_URI", 50) == 0) {
      config->spotify_creds.redirect_uri = strndup(val, 500);
    } else if (strncmp(key, "SPOTIFY_USER_REFRESH", 50) == 0) {
      config->spotify_creds.user_refresh_token = strndup(val, 500);
    } else {
      errprintf("Unknown config key: %s\n", key);
      fclose(fptr);
      return NULL;
    }
  }

  fclose(fptr);

  return config;
}

Config *load_config(const char *bin_name) {
  char *config_search_paths[] = {".", getenv("XDG_CONFIG_HOME"), "/etc"};

  for (size_t i = 0;
       i < (sizeof config_search_paths / sizeof config_search_paths[0]); i++) {
    // Try to load config from each location.
    char path[strlen(config_search_paths[i]) + strlen(bin_name)];
    strcpy(path, config_search_paths[i]);
    strcat(path, "/");
    strcat(path, bin_name);

    dbgprintf("attempting to read: %s\n", path);

    Config *config = read_config_from_file(path, bin_name);

    if (config != NULL) {
      return config;
    }
  }

  return NULL;
}

// int write_config_to_file(const char *path);
