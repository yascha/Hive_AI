#ifndef ALPHABETA_H
#define ALPHABETA_H

#include "debug.h"
#include "pieces.h"
#include "isWin.h"
#include "limits.h"
#include "evaluate.h"
#include "clock.h"
#ifdef ENABLE_TRANSPOSITION_TABLE
#include "zobrist.h"
#endif // ENABLE_TRANSPOSITION_TABLE

#define FIXED_DEPTH (4)
#define INFINITY (INT_MAX)

// globals
int numberEvaluations;
int numberNodes;
int currentDepth;
MOVE_t currMove;
int currentScore;

void calculateBestMove(bool whiteToPlay);
int alphaBeta(int depth, int alpha, int beta, bool whiteToPlay);
void initSearch(void);

#endif // ALPHABETA_H