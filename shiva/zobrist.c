

#include "zobrist.h"

uint64_t rand64(void)
{
    return rand() ^ ((uint64_t)rand() << 15) ^ ((uint64_t)rand() << 30) ^
	((uint64_t)rand() << 45) ^ ((uint64_t)rand() << 60);
}




int probeHash(int depth, int alpha, int beta)
{
    HASH_t *hash = &hash_table[currentZobristKey % HASH_TABLE_SIZE];
	
    if (hash->key == currentZobristKey) 
	{
        if (hash->depth >= depth) 
		{
            if (hash->flags == EXACT_HASH_RESULT)
                return hash->val;
            if ((hash->flags == ALPHA_HASH_RESULT) &&
                (hash->val <= alpha))
                return alpha;
            if ((hash->flags == BETA_HASH_RESULT) &&
                (hash->val >= beta))
                return beta;
        }
        //RememberBestMove();
    }
    return UNKNOWN_HASH_RESULT;
}

void recordHash(int depth, int val, int flags)
{
    HASH_t *hash = &hash_table[currentZobristKey % HASH_TABLE_SIZE];
	
    hash->key = currentZobristKey;
    hash->val = val;
	hash->flags = flags;
    hash->depth = depth;
}


uint64_t getZobristKey()
{
	return currentZobristKey;
}


void addPieceToZobristKey(PIECE_e piece)
{
	uint8_t colour, x, y;
	colour = isWhite(piece);
	x = getXpos(piece);
	y = getYpos(piece);

	currentZobristKey ^= zobrist[colour][piece][x][y];
	currentZobristKey ^= blackToPlayKey;
}


void zobristInit(void)
{
	// fill the zobrist key struct with random data
	uint32_t colour, piece, x, y;
	for (colour = 0; colour < MAX_COLOURS; colour++)
	{
		for (piece = 0; piece < MAX_PIECES; piece++)
		{
			for (x = 0; x < X_WIDTH; x++)
			{
				for (y = 0; y < Y_WIDTH; y++)
				{
					zobrist[colour][piece][x][y] = rand64();
				}
			}
		}
	}
	blackToPlayKey = rand64();
	currentZobristKey = 0;
	cacheHits = 0;
	cacheMisses = 0;
}
