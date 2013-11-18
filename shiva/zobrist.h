#ifndef ZOBRIST_H
#define ZOBRIST_H

#include "types.h"
#include "board.h"
#include "pieces.h"


#define removePieceFromZobristKey(piece) addPieceToZobristKey(piece)

#define HASH_TABLE_SIZE (2883584)
#define ZOBRIST_KEY_SIZE (2883584)

#define	   UNKNOWN_HASH_RESULT	0
#define    EXACT_HASH_RESULT	1
#define    ALPHA_HASH_RESULT	2
#define    BETA_HASH_RESULT		3

typedef struct {
    uint64_t key;
    int depth;
    int flags;
    int val;
    MOVE_t best;
}   HASH_t;


// globals
uint64_t zobrist[MAX_COLOURS][MAX_PIECES][X_WIDTH][Y_WIDTH];
uint64_t blackToPlayKey;
HASH_t hash_table[HASH_TABLE_SIZE];
uint64_t currentZobristKey;

uint32_t cacheHits;
uint32_t cacheMisses;

uint64_t rand64(void);
void recordHash(int depth, int val, int flags);
int probeHash(int depth, int alpha, int beta);
uint64_t getZobristKey();
void zobristInit(void);
void addPieceToZobristKey(PIECE_e piece);


#endif // ZOBRIST_H