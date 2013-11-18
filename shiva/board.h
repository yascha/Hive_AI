#ifndef BOARD_H
#define BOARD_H

#include "types.h"

#define X_WIDTH (256)
#define Y_WIDTH (256)

#define X(x) (x + 126)
#define Y(y) (y + 126)


// board to keep track of which squares are occupied
uint8_t board[256][256];

// piece array for when we need to check if the hive is broken
uint32_t pieceArray[22];

char* pieceNamePtr;
char* pieceQualifierPtr;
char* pieceAdjNamePtr;

uint8_t getOccupiedPositionsAroundCoords(uint8_t xpos, uint8_t ypos);
uint8_t getNumberOfOccupiedSquaresAroundCoords(uint8_t xpos, uint8_t ypos);
bool isHiveBroken(PIECE_e pieceToCheck);
bool isGate(uint8_t xpos, uint8_t ypos);
bool isGateOrUnnattached(uint8_t xpos, uint8_t ypos);
bool isSlidable(uint8_t x, uint8_t y, DIRECTIONS_e dir);
void initBoard(void);
void printBoard(int xSize, int ySize);
bool isSquareOccupied(uint8_t xpos, uint8_t ypos);
bool isTouchingCommonPiece(uint8_t x, uint8_t y, DIRECTIONS_e dir);
bool isTouchingOnlySameColor(bool isWhite, uint8_t x, uint8_t y);

#endif //BOARD_H