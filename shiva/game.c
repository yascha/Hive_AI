
#include "game.h"
#include "board.h"
#include "debug.h"
#include "pieces.h"
#include "isWin.h"

// example of notation to parse
//bot --times="300000,30,20" --moves='1. wB1, 2. bG1 -wB1, 3. wS1 wB1-, 4. bQ /bG1, 5. wA1 wB1\'

void parseMoveStringAndSetupBoardState(char* moveString)
{
	char* nextMove;	
	int moveNum;
	char pieceToMove[5];
	PIECE_e piece;
	char adjacentPiece[5];

	PIECE_e pieceAdjacent;
	DIRECTIONS_e adjacencyDirection;
	
	moveNumber = 1;
	
	// do it slightly different for the first move because:
	// 1. we need to clear the --moves=" part and 
	// 2.the first move doesn't have an adjacent target piece specifier
	nextMove = strtok(moveString, "=,");
	nextMove = strtok(NULL, "\",");
	if (nextMove == NULL || nextMove == ",")
	{
		printf("wS1");
		exit(0);
	}
	if (EOF == sscanf(nextMove, "%d. %5s", &moveNum, pieceToMove)) 	//1. wB1
	{
		printf("wS1");
		exit(0);
	}

	//DEBUG("Move #: %d\n Move String: %s\n", moveNumber, nextMove);
	
	piece = getPieceToMove(pieceToMove);
	
	// play the first move
	playFirstPiece(piece);
	moveNumber++;
	
	nextMove = strtok(NULL, ",");
	
	//MOVE_t moves[MOVE_LIST_SIZE];
	//MOVE_t *ptr = (MOVE_t*)malloc(sizeof(MOVE_t) * MOVE_LIST_SIZE);
	//&moves = ptr;
	
	// parse out all the moves
	while (nextMove != NULL && nextMove != ", ")
	{
		//DEBUG("Move #: %d\n Move String: %s\n", moveNumber, nextMove);
		
		if (EOF == sscanf(nextMove, "%d. %s %s", &moveNum, pieceToMove, adjacentPiece))
		{
			break;
		}
		
		piece = getPieceToMove(pieceToMove);
		pieceAdjacent = getAdjacentPiece(adjacentPiece, &adjacencyDirection);
		movePiece(piece, pieceAdjacent, adjacencyDirection);
		moveNumber++; 
		//printBoard(4, 4);
				 
		nextMove = strtok(NULL, ",");							
		
		
	}
}
// SOME TESTING STUFF YEEEEEEEEEEEEE:

/*
 uint32_t i;
 uint8_t x, y, origX, origY;
 bool beenPlayed = false;
 initMoves(moves);
 getQueenMoves(moves, true, 0); // wQ moves
 
 if (hasPieceBeenPlayed(WHITE_QUEEN))
 beenPlayed = true;
 else
 beenPlayed = false;
 
 //{
 i = 0;		
 // Play all of the valid queen moves and print the board to see them
 origX = getXpos(WHITE_QUEEN);
 origY = getYpos(WHITE_QUEEN);		
 while(moves[i].move != 0)
 {
 pickUpPiece(WHITE_QUEEN);
 x = (moves[i].move & XPOS_MASK) >> XPOS_SHIFT;
 y = (moves[i].move & YPOS_MASK) >> YPOS_SHIFT;
 putDownPiece(WHITE_QUEEN, x, y);
 printBoard(4, 4);
 i++;
 }
 pickUpPiece(WHITE_QUEEN);
 if (beenPlayed)
 putDownPiece(WHITE_QUEEN, origX, origY);
 printBoard(4, 4);
 //}
 
 initMoves(moves);
 getBeetleMoves(moves, WHITE_BEETLE_ONE, 0); //wB1 moves
 
 if (hasPieceBeenPlayed(WHITE_BEETLE_ONE))
 beenPlayed = true;
 else
 beenPlayed = false;
 //{			
 // Play all of the valid beetle moves and print the board to see them
 origX = getXpos(WHITE_BEETLE_ONE);
 origY = getYpos(WHITE_BEETLE_ONE);		
 i = 0;
 while(moves[i].move != 0)
 {
 pickUpPiece(WHITE_BEETLE_ONE);
 x = (moves[i].move & XPOS_MASK) >> XPOS_SHIFT;
 y = (moves[i].move & YPOS_MASK) >> YPOS_SHIFT;
 putDownPiece(WHITE_BEETLE_ONE, x, y);
 printBoard(4, 4);
 i++;
 }
 pickUpPiece(WHITE_BEETLE_ONE);
 if (beenPlayed)
 putDownPiece(WHITE_BEETLE_ONE, origX, origY);
 printBoard(4, 4);
 //}
 
 initMoves(moves);
 getGrasshopperMoves(moves, BLACK_GRASSHOPPER_ONE, 0); // bG1 moves
 
 if (hasPieceBeenPlayed(BLACK_GRASSHOPPER_ONE))
 beenPlayed = true;
 else
 beenPlayed = false;
 //{						
 // Play all of the valid grasshopper moves and print the board to see them
 origX = getXpos(BLACK_GRASSHOPPER_ONE);
 origY = getYpos(BLACK_GRASSHOPPER_ONE);		
 i = 0;
 while(moves[i].move != 0)
 {
 pickUpPiece(BLACK_GRASSHOPPER_ONE);
 x = (moves[i].move & XPOS_MASK) >> XPOS_SHIFT;
 y = (moves[i].move & YPOS_MASK) >> YPOS_SHIFT;
 putDownPiece(BLACK_GRASSHOPPER_ONE, x, y);
 printBoard(4, 4);
 i++;
 }
 pickUpPiece(BLACK_GRASSHOPPER_ONE);
 if (beenPlayed)
 putDownPiece(BLACK_GRASSHOPPER_ONE, origX, origY);
 printBoard(4, 4);
 //}
 
 initMoves(moves);
 getAntMoves(moves, WHITE_ANT_ONE, 0); // wA1 moves
 
 if (hasPieceBeenPlayed(WHITE_ANT_ONE))
 beenPlayed = true;
 else
 beenPlayed = false;
 //{			
 // Play all of the valid beetle moves and print the board to see them
 origX = getXpos(WHITE_ANT_ONE);
 origY = getYpos(WHITE_ANT_ONE);		
 i = 0;
 while(moves[i].move != 0)
 {
 pickUpPiece(WHITE_ANT_ONE);
 x = (moves[i].move & XPOS_MASK) >> XPOS_SHIFT;
 y = (moves[i].move & YPOS_MASK) >> YPOS_SHIFT;
 putDownPiece(WHITE_ANT_ONE, x, y);
 printBoard(4, 4);
 i++;
 }
 pickUpPiece(WHITE_ANT_ONE);
 if (beenPlayed)
 putDownPiece(WHITE_ANT_ONE, origX, origY);
 printBoard(4, 4);
 //}
 
 initMoves(moves);
 getSpiderMoves(moves, WHITE_SPIDER_ONE, 0); // wS1 moves
 
 if (hasPieceBeenPlayed(WHITE_SPIDER_ONE))
 beenPlayed = true;
 else
 beenPlayed = false;
 //{			
 // Play all of the valid beetle moves and print the board to see them
 origX = getXpos(WHITE_SPIDER_ONE);
 origY = getYpos(WHITE_SPIDER_ONE);		
 i = 0;
 while(moves[i].move != 0)
 {
 pickUpPiece(WHITE_SPIDER_ONE);
 x = (moves[i].move & XPOS_MASK) >> XPOS_SHIFT;
 y = (moves[i].move & YPOS_MASK) >> YPOS_SHIFT;
 putDownPiece(WHITE_SPIDER_ONE, x, y);
 printBoard(4, 4);
 i++;
 }
 pickUpPiece(WHITE_SPIDER_ONE);
 if (beenPlayed)
 putDownPiece(WHITE_SPIDER_ONE, origX, origY);
 printBoard(4, 4);
 //}
 initMoves(moves);
 */

PIECE_e getPieceToMove(char* pieceToMove)
{
	char* colorToMove;
	char* numberToMove;
	char* typeToMove;
	
	PIECE_e piece;
	
	// get the piece index from the parts
	colorToMove = &pieceToMove[0];
	typeToMove = &pieceToMove[1];
	numberToMove = &pieceToMove[2];
	
	piece = translateTypeAndNumberToIndex(*colorToMove, *typeToMove, *numberToMove);
	return piece;
	
}

PIECE_e getAdjacentPiece(char* adjacentPiece, DIRECTIONS_e* adjacencyDirection)
{
	char* colorToMove;
	char* numberToMove;
	char* typeToMove;
	
	PIECE_e piece;
	
	// get the adjacent piece index from the parts
	if (WEST_ADJ == adjacentPiece[0])
	{
		colorToMove = &adjacentPiece[1];
		typeToMove = &adjacentPiece[2];
		numberToMove = &adjacentPiece[3];
		*adjacencyDirection = WEST;
	}
	else if (NORTHWEST_ADJ == adjacentPiece[0])
	{
		colorToMove = &adjacentPiece[1];
		typeToMove = &adjacentPiece[2];
		numberToMove = &adjacentPiece[3];
		*adjacencyDirection = NORTHWEST;
	}
	else if (SOUTHWEST_ADJ == adjacentPiece[0])
	{
		colorToMove = &adjacentPiece[1];
		typeToMove = &adjacentPiece[2];
		numberToMove = &adjacentPiece[3];					 
		*adjacencyDirection = SOUTHWEST;
	}
	else
	{
		colorToMove = &adjacentPiece[0];
		typeToMove = &adjacentPiece[1];
		numberToMove = &adjacentPiece[2];

		if(*typeToMove == 'Q')
		{
			if (NORTHEAST_ADJ == adjacentPiece[2])
				*adjacencyDirection = NORTHEAST;
			else if (EAST_ADJ == adjacentPiece[2])
				*adjacencyDirection = EAST;
			else if (SOUTHEAST_ADJ == adjacentPiece[2])
				*adjacencyDirection = SOUTHEAST;
			else 
				*adjacencyDirection = MAX_DIR; // MAX_DIR indicates to land on top
		}
		else
		{
			if (NORTHEAST_ADJ == adjacentPiece[3])
				*adjacencyDirection = NORTHEAST;
			else if (EAST_ADJ == adjacentPiece[3])
				*adjacencyDirection = EAST;
			else if (SOUTHEAST_ADJ == adjacentPiece[3])
				*adjacencyDirection = SOUTHEAST;
			else 
				*adjacencyDirection = MAX_DIR; // MAX_DIR indicates to land on top
		}
	}

	piece = translateTypeAndNumberToIndex(*colorToMove, *typeToMove, *numberToMove);
	
	return piece;
}

void printBoard(int xSize, int ySize)
{
	int x, y, z;
	char** pptr = &pieceNamePtr;
	
	int spaceCount = ySize * 2;
	for (y = Y(ySize); y >= Y(-1*ySize); y--)
	{
		for (z = spaceCount; z > 0; z--)
			fprintf(stderr, "   ");
		spaceCount--;
		for (x = X(-1*xSize); x <= X(xSize); x++)
		{
			if (board[x][y] == EMPTY_SPACE)
			{
				fprintf(stderr, "  -  ");
			}
			else 
			{
				getPieceName(board[x][y], pptr);
				fprintf(stderr, " %s ", pieceNamePtr);
			}
		}
		fprintf(stderr, "\n");
	}
}


void getPieceName(uint8_t piece, char** pieceName)
{
	switch (piece)
	{
		case WHITE_QUEEN:
			*pieceName = "wQ";
			break;
			
		case BLACK_QUEEN:
			*pieceName = "bQ";
			break;
			
		case WHITE_BEETLE_ONE:
			*pieceName = "wB1";
			break;
			
		case WHITE_BEETLE_TWO:
			*pieceName = "wB2";
			break;
			
		case BLACK_BEETLE_ONE:
			*pieceName = "bB1";
			break;
			
		case BLACK_BEETLE_TWO:
			*pieceName = "bB2";
			break;
			
		case WHITE_GRASSHOPPER_ONE:
			*pieceName = "wG1";
			break;
			
		case WHITE_GRASSHOPPER_TWO:
			*pieceName = "wG2";
			break;
			
		case WHITE_GRASSHOPPER_THREE:
			*pieceName = "wG3";
			break;
			
		case BLACK_GRASSHOPPER_ONE:
			*pieceName = "bG1";
			break;
			
		case BLACK_GRASSHOPPER_TWO:
			*pieceName = "bG2";
			break;
			
		case BLACK_GRASSHOPPER_THREE:
			*pieceName = "bG3";
			break;
			
		case WHITE_ANT_ONE:
			*pieceName = "wA1";
			break;
			
			
		case WHITE_ANT_TWO:
			*pieceName = "wA2";
			break;
			
		case WHITE_ANT_THREE:
			*pieceName = "wA3";
			break;
			
		case BLACK_ANT_ONE:
			*pieceName = "bA1";
			break;
			
		case BLACK_ANT_TWO:
			*pieceName = "bA2";
			break;
			
		case BLACK_ANT_THREE:
			*pieceName = "bA3";
			break;
			
		case WHITE_SPIDER_ONE:
			*pieceName = "wS1";
			break;
			
		case WHITE_SPIDER_TWO:
			*pieceName = "wS2";
			break;
			
		case BLACK_SPIDER_ONE:
			*pieceName = "bS1";
			break;
			
		case BLACK_SPIDER_TWO:
			*pieceName = "bS2";
			break;
			
		default:
			*pieceName = "???";
			break;
	}
}


// return true if qualifier is a prefix, false if it's a postfix
bool getAdjacentPieceAndQualifier(PIECE_e piece, uint8_t x, uint8_t y, char** pieceName, char** qualifier)
{
	
	// check adjacent positions for a piece to use as a qualifier
	
	// NE
	if (board[xNE(x)][yNE(y)] < MAX_PIECES && board[xNE(x)][yNE(y)] != piece)
	{
		getPieceName(board[xNE(x)][yNE(y)], pieceName);
		*qualifier = NORTHEAST_STR;
		return true;
	}
	// E
	else if (board[xE(x)][yE(y)] < MAX_PIECES && board[xE(x)][yE(y)] != piece)
	{
		getPieceName(board[xE(x)][yE(y)], pieceName);
		*qualifier = EAST_STR;
		return true;
	}
	// SE
	else if (board[xSE(x)][ySE(y)] < MAX_PIECES && board[xSE(x)][ySE(y)] != piece)
	{
		getPieceName(board[xSE(x)][ySE(y)], pieceName);	
		*qualifier = SOUTHEAST_STR;
		return true;
	}
	// SW
	else if (board[xSW(x)][ySW(y)] < MAX_PIECES && board[xSW(x)][ySW(y)] != piece)
	{
		getPieceName(board[xSW(x)][ySW(y)], pieceName);	
		*qualifier = SOUTHWEST_STR;
		return false;
	}
	// W
	else if (board[xW(x)][yW(y)] < MAX_PIECES && board[xW(x)][yW(y)] != piece)
	{
		getPieceName(board[xW(x)][yW(y)], pieceName);		
		*qualifier = WEST_STR;
		return false;
	}
	// NW
	else if (board[xNW(x)][yNW(y)] < MAX_PIECES && board[xNW(x)][yNW(y)] != piece)
	{
		getPieceName(board[xNW(x)][yNW(y)], pieceName);
		*qualifier = NORTHWEST_STR;
		return false;
	}

	return false;
}

void printOutBestMove(void)
{
	PIECE_e piece;
	uint8_t x, y;
	char** pptr;
	char** pptr2;
	char** pptr3;
	
	pptr = &pieceNamePtr;
	pptr2 = &pieceQualifierPtr;
	pptr3 = &pieceAdjNamePtr;
	
	piece = ((currMove.move & PIECE_MASK) >> PIECE_SHIFT);
	x = ((currMove.move & XPOS_MASK) >> XPOS_SHIFT);
	y = ((currMove.move & YPOS_MASK) >> YPOS_SHIFT);
	
	getPieceName(piece, pptr);
	
	// print the piece to move
	//fprintf(stderr, "%s ", pieceNamePtr);

	pickUpPiece(piece);
	
	if(getAdjacentPieceAndQualifier(piece, x, y, pptr3, pptr2))
	{
		printf("%s %s%s", pieceNamePtr, pieceQualifierPtr, pieceAdjNamePtr);
		//DEBUG("%s %s%s", pieceNamePtr, pieceQualifierPtr, pieceAdjNamePtr);
	}
	else 
	{
		printf( "%s %s%s", pieceNamePtr, pieceAdjNamePtr, pieceQualifierPtr);
		//DEBUG("%s %s%s", pieceNamePtr, pieceAdjNamePtr, pieceQualifierPtr);
	}
}

void initState(void)
{
	initPieces();
	initBoard();
	initSearch();
	initPinnedMap();
#if ENABLE_TRANSPOSITION_TABLE
	zobristInit();
#endif
	pieceNamePtr = (char*)malloc(sizeof(char)*5);
	pieceQualifierPtr = (char*)malloc(sizeof(char)*3);
	pieceAdjNamePtr = (char*)malloc(sizeof(char)*5);
	if (pieceNamePtr == NULL)
	{
		printf("ERROR: Could not allocate char ptr\n");
	}
}
