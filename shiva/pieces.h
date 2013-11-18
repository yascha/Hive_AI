#ifndef PIECES_H
#define PIECES_H

#include "debug.h"
#include "types.h"
#include "board.h"
#include "game.h"

typedef enum
{
    NE_SHIFT = 0,
    E_SHIFT,
    SE_SHIFT,
    SW_SHIFT,
    W_SHIFT,
    NW_SHIFT,
    MAX_SHIFT
} DIRECTION_SHIFT_e;
	

#define xNE(x) (x)
#define yNE(y) (y+1)
#define xE(x) (x+1)
#define yE(y) (y)
#define xSE(x) (x+1)
#define ySE(y) (y-1)
#define xSW(x) (x)
#define ySW(y) (y-1)
#define xW(x) (x-1)
#define yW(y) (y)
#define xNW(x) (x-1)
#define yNW(y) (y+1)

// get a struct to hold all of the pieces
// pzzzzzyyyyyyyyxxxxxxxxBW321SAGBQ
PIECE_t pieces[MAX_PIECES];

#define XCOORD_MASK (0x0003FC00)
#define YCOORD_MASK (0x03FC0000)
#define ZCOORD_MASK (0x7C000000)

#define QUEEN_MASK (0x00000001)
#define BEETLE_MASK (0x00000002)
#define GRASSHOPPER_MASK (0x00000004)
#define ANT_MASK (0x00000008)
#define SPIDER_MASK (0x00000010)
#define NUMBER_MASK (0x000000E0)
#define COLOR_MASK (0x00000300)

#define BEEN_PLAYED_MASK (0x80000000)

#define XCOORD_SHIFT (10)
#define YCOORD_SHIFT (18)
#define ZCOORD_SHIFT (26)
#define QUEEN_SHIFT (0)
#define BEETLE_SHIFT (1)
#define GRASSHOPPER_SHIFT (2)
#define ANT_SHIFT (3)
#define SPIDER_SHIFT (4)
#define NUMBER_SHIFT (5)
#define COLOR_SHIFT (8)
#define BEEN_PLAYED_SHIFT (31)

#define ZPOS_ALONE (1)


#define MOVE_LIST_SIZE (512)

// get structure to hold candidate moves
// --------ppppppppyyyyyyyyxxxxxxxx;
//MOVE_t moves[MOVE_LIST_SIZE];
			 
#define XPOS_SHIFT (0)
#define YPOS_SHIFT (8)
#define PIECE_SHIFT (16)
#define XPOS_MASK (0x000000FF)
#define YPOS_MASK (0x0000FF00)
#define PIECE_MASK (0x00FF0000)



void pickUpPiece(PIECE_e piece);
void putDownPiece(PIECE_e piece, uint8_t xpos, uint8_t ypos);
void playFirstPiece(PIECE_e piece);
void movePiece(PIECE_e pieceToMove, PIECE_e adjacentPiece, DIRECTIONS_e adjacencyType);
void playMove(MOVE_t *moves, uint32_t index);
void undoMove(PIECE_t originalPiece, PIECE_e piece);

PIECE_e translateTypeAndNumberToIndex(char color, char type, char number);

void initPieces(void);

uint8_t getXpos(PIECE_e piece);
uint8_t getYpos(PIECE_e piece);
uint8_t getZpos(PIECE_e piece);

void setXpos(PIECE_e piece, uint8_t xpos);
void setYpos(PIECE_e piece, uint8_t ypos);
void setZpos(PIECE_e piece, uint8_t zpos);

void setColor(PIECE_e piece, bool white);

bool isBeetle(PIECE_e piece);
bool isSpider(PIECE_e piece);
bool isQueen(PIECE_e piece);
bool isAnt(PIECE_e piece);
bool isGrasshopper(PIECE_e piece);

bool isWhite(PIECE_e piece);

void initMoves(MOVE_t *moves);

bool hasPieceBeenPlayed(PIECE_e piece);
void setPieceHasBeenPlayed(PIECE_e piece, bool placed);

bool isStuckUnderABeetle(PIECE_e piece);
bool isAlmostSurrounded(PIECE_e piece);

uint32_t getQueenMoves(MOVE_t* moves, PIECE_e piece, uint32_t index);
uint32_t getBeetleMoves(MOVE_t *moves, PIECE_e piece, uint32_t index);
uint32_t getGrasshopperMoves(MOVE_t *moves, PIECE_e piece, uint32_t index);
uint32_t getAntMoves(MOVE_t *moves, PIECE_e piece, uint32_t index);
uint32_t getSpiderMoves(MOVE_t *moves, PIECE_e piece, uint32_t index);
uint32_t getStartingPositions(MOVE_t *moves, PIECE_e piece, uint32_t startIndex);

uint32_t AddAllLegalMovesToMoveList(MOVE_t *moves, EVALUATE_INFO_t *eval, bool whiteToPlay);

#endif // PIECES_H