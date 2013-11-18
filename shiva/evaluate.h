#ifndef EVALUATE_H
#define EVALUATE_H

#include "types.h"
#include "pieces.h"
#include "isWin.h"

#define FIVE_SQUARES_AROUND_QUEEN_SCORE (4000)
#define FOUR_SQUARES_AROUND_QUEEN_SCORE (2000)
#define THREE_SQUARES_AROUND_QUEEN_SCORE (400)
#define TWO_SQUARES_AROUND_QUEEN_SCORE (0)

#define QUEEN_PINNED_SCORE (400)
#define BEETLE_PINNED_SCORE (80)
#define SPIDER_PINNED_SCORE (30)
#define ANT_PINNED_SCORE (50)
#define GRASSHOPPER_PINNED_SCORE (40)

// globals
int pinnedMap[MAX_PIECES];

int evaluateBoard(EVALUATE_INFO_t *eval);
void initPinnedMap(void);

#endif //EVALUATE_H