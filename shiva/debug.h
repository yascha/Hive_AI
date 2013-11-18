#ifndef DEBUG_H
#define DEBUG_H


#include <stdio.h>

#define ENABLE_TRANSPOSITION_TABLE 0

#define DEBUG_ENABLE 1

#define DEBUG(fmt, ...) \
do { if (DEBUG_ENABLE) fprintf(stderr, fmt, __VA_ARGS__); } while (0)


#endif // DEBUG_H