#ifndef TIME_H
#define TIME_H

#include <stdlib.h>
#include <string.h>
#include "types.h"

uint32_t totalGameTimePerPlayer;
uint32_t whiteRemainingTime;
uint32_t blackRemainingTime;

void parseTimeString(char* timeString);

#endif