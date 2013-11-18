#ifndef TYPES_H
#define TYPES_H

#include "debug.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef enum
{
	BLACK = 0,
	WHITE,
	MAX_COLOURS
} COLOURS_e;


typedef enum
{
	NORTHEAST = 0,
	EAST,
	SOUTHEAST,
	SOUTHWEST,
	WEST,
	NORTHWEST,
	MAX_DIR
} DIRECTIONS_e;


typedef enum
{
	WHITE_QUEEN = 0,
	WHITE_BEETLE_ONE,
	WHITE_BEETLE_TWO,
	WHITE_GRASSHOPPER_ONE,
	WHITE_GRASSHOPPER_TWO,
	WHITE_GRASSHOPPER_THREE,
	WHITE_ANT_ONE,
	WHITE_ANT_TWO,
	WHITE_ANT_THREE,
	WHITE_SPIDER_ONE,
	WHITE_SPIDER_TWO,	
	BLACK_QUEEN,
	BLACK_BEETLE_ONE,
	BLACK_BEETLE_TWO,
	BLACK_GRASSHOPPER_ONE,
	BLACK_GRASSHOPPER_TWO,
	BLACK_GRASSHOPPER_THREE,
	BLACK_ANT_ONE,
	BLACK_ANT_TWO,
	BLACK_ANT_THREE,
	BLACK_SPIDER_ONE,
	BLACK_SPIDER_TWO,
	MAX_PIECES,
	EMPTY_SPACE
}PIECE_e;

typedef struct
{
	uint32_t validMoves[MAX_PIECES];
	uint32_t whiteMoves;
	uint32_t blackMoves;
} EVALUATE_INFO_t;


#if DEBUG_ENABLE

/* bitwise breakdown of the piece struct:
 
 5 piece types QBGAS (0-4)
 3 numbers 123 (5-7)
 2 color bw (8-9)
 8 xpos (10-17)
 8 ypos (18-25)
 5 zpos (26-30)
 1 beenPlayed (31) */
typedef union
{
	struct 
	{
		uint32_t type : 5;
		uint32_t number : 3;
		uint32_t color : 2;
		uint32_t xcoords : 8;
		uint32_t ycoords : 8;
		uint32_t zcoords : 5;
		uint32_t played : 1;
	} PIECE_BITS_t;
	uint32_t piece;
} PIECE_t;

typedef union
{
	struct 
	{
		uint32_t xpos : 8;
		uint32_t ypos : 8;
		PIECE_e piece : 8;
		uint32_t reserved : 8;
	} MOVE_BITS_t;
	uint32_t move;
} MOVE_t;

#else

typedef struct 
{
	uint32_t piece;
} PIECE_t;

typedef struct
{
	uint32_t move; // --------ppppppppyyyyyyyyxxxxxxxx;
} MOVE_t;

#endif // DEBUG_ENABLE



#endif // TYPES_H