#pragma once

#include "auth.h"

typedef struct Config {
  char *bin_name;
  SpotifyCredentials spotify_creds;
} Config;

// Config *new_config(const char *bin_name);
// Config *read_config_from_file(const char *path, const char *bin_name);
Config *load_config(const char *bin_name);
// int write_config_to_file(const char *path, const Config *config);
