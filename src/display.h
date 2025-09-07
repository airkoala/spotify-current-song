#pragma once

#include "playback_status.h"

void build_output(char *dst, const PlaybackStatus *pbs);
char *build_scroller(char *dst, const PlaybackStatus *pbs, uint32_t length, uint32_t step_delay_ms);
