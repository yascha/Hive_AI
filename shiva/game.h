#ifndef GAME_H
#define GAME_H

#include <string.h>
#include "types.h"
#include "pieces.h"
#include "board.h"
#include "alphabeta.h"
#include "zobrist.h"

// postfixes
#define NORTHEAST_ADJ	'/'
#define NORTHEAST_STR	"/"
#define EAST_ADJ		'-'
#define EAST_STR		"-"
#define SOUTHEAST_ADJ	'\\'
#define SOUTHEAST_STR	"\\"

// prefixes
#define SOUTHWEST_ADJ	'/'
#define SOUTHWEST_STR	"/"
#define WEST_ADJ		'-'
#define WEST_STR		"-"
#define NORTHWEST_ADJ	'\\'
#define NORTHWEST_STR	"\\"

// globals
uint32_t moveNumber;

void parseMoveStringAndSetupBoardState(char* moveString);
PIECE_e getPieceToMove(char* pieceToMove);
PIECE_e getAdjacentPiece(char* adjacentPiece, DIRECTIONS_e* adjacencyDirection);
void getPieceName(uint8_t piece, char** pieceName);
bool getAdjacentPieceAndQualifier(PIECE_e piece, uint8_t x, uint8_t y, char** pieceName, char** qualifier);
void printOutBestMove(void);
void initState(void);


//'1. wB1, 2. bG1 -wB1, 3. wS1 wB1-, 4. bQ /bG1, 5. wA1 wB1\'

#endif // GAME_H