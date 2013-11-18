
#include "board.h"
#include "pieces.h"

static uint32_t explorePieces(PIECE_e piece);

// return the locations of the occupied positions around the given xy coordinates
// XX(NW)(W)(SW)(SE)(E)(NE)   ie. 00000101 would mean there are pieces NE and SE
uint8_t getOccupiedPositionsAroundCoords(uint8_t xpos, uint8_t ypos)
{
	uint8_t retVal = 0;
	
	if (board[xpos][ypos+1] != EMPTY_SPACE)
		retVal |= (1 << NE_SHIFT);
	if (board[xpos+1][ypos] != EMPTY_SPACE) 
		retVal |= (1 << E_SHIFT);
	if (board[xpos+1][ypos-1] != EMPTY_SPACE)
		retVal |= (1 << SE_SHIFT);
	if (board[xpos][ypos-1] != EMPTY_SPACE)
		retVal |= (1 << SW_SHIFT);
	if (board[xpos-1][ypos] != EMPTY_SPACE) 
		retVal |= (1 << W_SHIFT);
	if (board[xpos-1][ypos+1] != EMPTY_SPACE)
		retVal |= (1 << NW_SHIFT);
	return retVal;
}


uint8_t getNumberOfOccupiedSquaresAroundCoords(uint8_t xpos, uint8_t ypos)
{
	uint8_t sum = 0;
	uint8_t retVal = getOccupiedPositionsAroundCoords(xpos, ypos);
	for (int i = 0; i < 6; i++)
	{
		if ((retVal >> i) & 1)
			sum +=1;
	}
	return sum;
}


void initBoard(void)
{
	// TODO: Do I need to do this?
	int i, j;
	for (i = 0; i < 256; i++)
	{
		for (j = 0; j < 256; j++)
		{
			board[i][j] = EMPTY_SPACE;
		}
	}
}


bool isHiveBroken(PIECE_e pieceToCheck)
{
	PIECE_e piece, startingPiece;
	uint8_t xpos, ypos;
	uint32_t numberOfPiecesPlayed = 0;
	uint32_t piecesExplored = 0;
	
	// find out how many pieces are on the board
	for (piece = 0; piece < MAX_PIECES; piece++)
	{
		pieceArray[piece] = 0;
		if (hasPieceBeenPlayed(piece))
		{
			numberOfPiecesPlayed++;
			pieceArray[piece] = 1;
			startingPiece = piece;
		}
	}
	
//	xpos = getXpos(startingPiece);
//	ypos = getYpos(startingPiece);
//	pieceArray[startingPiece] = 0;
//	piecesExplored++;

	xpos = getXpos(pieceToCheck);
	ypos = getYpos(pieceToCheck);
	
	// NORTHEAST
	if (board[xpos][ypos+1] < MAX_PIECES && pieceArray[board[xpos][ypos+1]])
		piecesExplored += explorePieces(board[xpos][ypos+1]);
	// EAST
	else if (board[xpos+1][ypos] < MAX_PIECES && pieceArray[board[xpos+1][ypos]])
		piecesExplored += explorePieces(board[xpos+1][ypos]);
	// SOUTHEAST
	else if (board[xpos+1][ypos-1] < MAX_PIECES && pieceArray[board[xpos+1][ypos-1]])
		piecesExplored += explorePieces(board[xpos+1][ypos-1]);
	// SOUTHWEST
	else if (board[xpos][ypos-1] < MAX_PIECES && pieceArray[board[xpos][ypos-1]])
		piecesExplored += explorePieces(board[xpos][ypos-1]);
	// WEST
	else if (board[xpos-1][ypos] < MAX_PIECES && pieceArray[board[xpos-1][ypos]])
		piecesExplored += explorePieces(board[xpos-1][ypos]);
	// NORTHWEST
	else if (board[xpos-1][ypos+1] < MAX_PIECES && pieceArray[board[xpos-1][ypos+1]])
		piecesExplored += explorePieces(board[xpos-1][ypos+1]);
	
	if (numberOfPiecesPlayed != piecesExplored)
		return true;		

	return false;
}


static uint32_t explorePieces(PIECE_e piece)
{
	uint32_t piecesExplored = 0;
	uint8_t xpos, ypos, zpos;
	PIECE_e neighbour;
	
	// indicate that piece has already been explored
	pieceArray[piece] = 0;
	piecesExplored++;
	
	xpos = getXpos(piece);
	ypos = getYpos(piece);
	zpos = getZpos(piece);
	
	if (zpos > ZPOS_ALONE)
	{
		while (!(zpos & 1))
		{
			zpos = zpos >> 1;
			piecesExplored++;
		}
	}
	
	// NORTHEAST
	neighbour = board[xpos][ypos+1];
	if (neighbour < MAX_PIECES && pieceArray[neighbour])
		piecesExplored += explorePieces(neighbour);

	// EAST
	neighbour = board[xpos+1][ypos];
	if (neighbour < MAX_PIECES && pieceArray[neighbour])
		piecesExplored += explorePieces(neighbour);
	
	// SOUTHEAST
	neighbour = board[xpos+1][ypos-1];
	if (neighbour < MAX_PIECES && pieceArray[neighbour])
		piecesExplored += explorePieces(neighbour);
	
	// SOUTHWEST
	neighbour = board[xpos][ypos-1];
	if (neighbour < MAX_PIECES && pieceArray[neighbour])
		piecesExplored += explorePieces(neighbour);
	
	// WEST
	neighbour = board[xpos-1][ypos];
	if (neighbour < MAX_PIECES && pieceArray[neighbour])
		piecesExplored += explorePieces(neighbour);

	// NORTHWEST
	neighbour = board[xpos-1][ypos+1];
	if (neighbour < MAX_PIECES && pieceArray[neighbour])
		piecesExplored += explorePieces(neighbour);

	return piecesExplored;
}


inline bool isGate(uint8_t xpos, uint8_t ypos)
{
	return (5 == getNumberOfOccupiedSquaresAroundCoords(xpos, ypos));
}


inline bool isGateOrUnnattached(uint8_t xpos, uint8_t ypos)
{
	uint8_t temp = getNumberOfOccupiedSquaresAroundCoords(xpos, ypos);
	if (temp == 5 || temp == 0)
		return true;
	return false;
}

inline bool isSquareOccupied(uint8_t xpos, uint8_t ypos)
{
	if (board[xpos][ypos] != EMPTY_SPACE)
		return true;
	return false;
}

inline bool isSlidable(uint8_t x, uint8_t y, DIRECTIONS_e dir)
{
	if (NORTHEAST == dir)
	{
		// east and northwest 
		if ((board[xE(x)][yE(y)] < MAX_PIECES) && (board[xNW(x)][yNW(y)] < MAX_PIECES))
			return false;
		return true;
	}
	else if (EAST == dir)
	{
		// northeast and southeast
		if ((board[xNE(x)][yNE(y)] < MAX_PIECES) && (board[xSE(x)][ySE(y)] < MAX_PIECES))
			return false;
		return true;
	}
	else if (SOUTHEAST == dir)
	{
		// southwest and east
		if ((board[xSW(x)][ySW(y)] < MAX_PIECES) && (board[xE(x)][yE(y)] < MAX_PIECES))
			return false;
		return true;
	}
	else if (SOUTHWEST == dir)
	{
		//  southeast and  west
		if ((board[xSE(x)][ySE(y)] < MAX_PIECES) && (board[xW(x)][yW(y)] < MAX_PIECES))
			return false;
		return true;
	}
	else if (WEST == dir)
	{
		//  northwest and southwest
		if ((board[xNW(x)][yNW(y)] < MAX_PIECES) && (board[xSW(x)][ySW(y)] < MAX_PIECES))
			return false;
		return true;
	}
	else// if (NORTHWEST == dir)
	{
		//  northeast and west
		if ((board[xNE(x)][yNE(y)] < MAX_PIECES) && (board[xW(x)][yW(y)] < MAX_PIECES))
			return false;
		return true;
	}
}

// x and y are starting positions
inline bool isTouchingCommonPiece(uint8_t x, uint8_t y, DIRECTIONS_e dir)
{
	if (NORTHEAST == dir)
	{
		// Can only share east or northwest piece
		if (board[xE(x)][yE(y)] < MAX_PIECES)
			return true;
		else if (board[xNW(x)][yNW(y)] < MAX_PIECES)
			return true;
		return false;
	}
	else if (EAST == dir)
	{
		// can only share northeast or southeast
		if (board[xNE(x)][yNE(y)] < MAX_PIECES)
			return true;
		else if (board[xSE(x)][ySE(y)] < MAX_PIECES)
			return true;
		return false;
	}
	else if (SOUTHEAST == dir)
	{
		// can only share southwest or east
		if (board[xSW(x)][ySW(y)] < MAX_PIECES)
			return true;
		else if (board[xE(x)][yE(y)] < MAX_PIECES)
			return true;
		return false;
	}
	else if (SOUTHWEST == dir)
	{
		// can only share southeast or west
		if (board[xSE(x)][ySE(y)] < MAX_PIECES)
			return true;
		else if (board[xW(x)][yW(y)] < MAX_PIECES)
			return true;
		return false;
	}
	else if (WEST == dir)
	{
		// can only share northwest or southwest
		if (board[xNW(x)][yNW(y)] < MAX_PIECES)
			return true;
		else if (board[xSW(x)][ySW(y)] < MAX_PIECES)
			return true;
		return false;
	}
	else// if (NORTHWEST == dir)
	{
		// can only border northeast or west
		if (board[xNE(x)][yNE(y)] < MAX_PIECES)
			return true;
		else if (board[xW(x)][yW(y)] < MAX_PIECES)
			return true;
		return false;
	}
}

// assume coords are touching something; if not, will return true
bool isTouchingOnlySameColor(bool white, uint8_t x, uint8_t y)
{
	// NE
	if (board[xNE(x)][yNE(y)] < MAX_PIECES)	
	{
		if (white != isWhite(board[xNE(x)][yNE(y)]))
			return false;		
	}
		
	// E
	if (board[xE(x)][yE(y)] < MAX_PIECES)
	{
		if (white != isWhite(board[xE(x)][yE(y)]))
			return false;
	}
	
	// SE
	if (board[xSE(x)][ySE(y)] < MAX_PIECES)
	{
		if (white != isWhite(board[xSE(x)][ySE(y)]))
			return false;
	}
		
	// SW
	if (board[xSW(x)][ySW(y)] < MAX_PIECES)
	{		
		if (white != isWhite(board[xSW(x)][ySW(y)]))
			return false;		
	}
	
	// W
	if (board[xW(x)][yW(y)] < MAX_PIECES)
	{
		if (white != isWhite(board[xW(x)][yW(y)]))
			return false;
	}

	// NW
	if (board[xNW(x)][yNW(y)] < MAX_PIECES)
	{
		if (white != isWhite(board[xNW(x)][yNW(y)]))
			return false;
		
	}
	return true;	
}


