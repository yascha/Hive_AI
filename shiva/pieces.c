
#include "pieces.h"
#include "board.h"

static uint8_t getHighestZposAtPosition(uint8_t xpos, uint8_t ypos, PIECE_e piece);
static uint32_t antSearch(MOVE_t *moves, PIECE_e piece, uint32_t startIndex, uint8_t x, uint8_t y, uint32_t index);
static uint32_t spiderSearch(MOVE_t *moves, PIECE_e piece, uint32_t startIndex, uint8_t x, uint8_t y, uint32_t index, uint32_t depth, uint8_t prevX, uint8_t prevY);
static bool isMoveAlreadyOnMoveList(MOVE_t *moves, uint32_t startIndex, uint32_t index, MOVE_t move);
static inline bool isPreviousMoveCoords(MOVE_t *moves, uint32_t index, uint8_t xpos, uint8_t ypos);
static uint32_t startPositionSearch(MOVE_t *moves, PIECE_e piece, PIECE_e pieceToPlay, uint32_t startIndex, uint32_t currentIndex);
static uint32_t getMoveTwoMoves(MOVE_t *moves);

void pickUpPiece(PIECE_e piece)
{
    uint8_t origzpos = getZpos(piece);
    uint8_t origxpos = getXpos(piece);
    uint8_t origypos = getYpos(piece);
	uint8_t xpos, ypos, zpos;
	uint8_t maxZposTemp = 0;
	
	setPieceHasBeenPlayed(piece, false);
	
    // if it was on top of other pieces, then we need to update the board state to show the piece below it
    if (origzpos != ZPOS_ALONE)
    {
        for (PIECE_e i = 0; i < MAX_PIECES; i++)
        {
			if (hasPieceBeenPlayed(i) && i != piece)
			{
				xpos = getXpos(i);
				ypos = getYpos(i);
				
				if ((xpos == origxpos) && (ypos == origypos))
				{
					zpos = getZpos(i);
					// if it's the top piece, set the board state to reflect that
					if (zpos > maxZposTemp)
					{
						maxZposTemp = zpos;
						board[xpos][ypos] = i;						
					}
				}		
			}
        }
    }
    else
    {
        // update the game board to clear the space we just vacated
        board[origxpos][origypos] = EMPTY_SPACE;
    }
	
#if ENABLE_TRANSPOSITION_TABLE
	
#endif //ENABLE_TRANSPOSITION_TABLE	
}




void putDownPiece(PIECE_e piece, uint8_t xpos, uint8_t ypos)
{
    // assume we don't need to do any validity checks here for now
	
    if (isBeetle(piece) && (board[xpos][ypos] < MAX_PIECES))
    {
        // we are a beetle and we are landing on something
		// find the highest piece and set our zcoord to be higher
		uint8_t zpos = getHighestZposAtPosition(xpos, ypos, piece);
		setZpos(piece, (zpos<<1));
		//getPieceName(piece, &pieceNamePtr);
		//DEBUG("Setting ZPOS of %d for piece %s\n", (zpos<<1), pieceNamePtr);

//		// iterate through the pieces and update anyone we land on
//		uint8_t xposTemp, yposTemp, zposTemp;
//		
//        for (int i = 0; i < MAX_PIECES; i++)
//        {
//            xposTemp = getXpos(i);
//            yposTemp = getYpos(i);
//			
//            if ((xpos == xposTemp) && (ypos == yposTemp) && (i != piece))
//            {
//                zposTemp = getZpos(i);
//				// move the piece down and replaces its zpos
//				zposTemp = zposTemp << 1;
//				setZpos(i, zposTemp);
//            }
//        }
    }
	else
	{
		setZpos(piece, ZPOS_ALONE);	
	}
	
	setXpos(piece, xpos);
	setYpos(piece, ypos);
	
	//update the board position with this piece
	board[xpos][ypos] = piece;
	setPieceHasBeenPlayed(piece, true);
}


static uint8_t getHighestZposAtPosition (uint8_t xpos, uint8_t ypos, PIECE_e piece)
{
	PIECE_e i;
	uint8_t maxZ = 0;
	for (i = 0; i < MAX_PIECES; i++)
	{
		if ((getXpos(i) == xpos) && (getYpos(i) == ypos) && (i != piece))
		{
			if (getZpos(i) > maxZ)
			{
				maxZ = getZpos(i);
				//getPieceName(i, &pieceNamePtr);
				//DEBUG("MAXZ = %d with piece = %s\n", getZpos(i), pieceNamePtr);
			}
		}
	}
	return maxZ;
}

void playFirstPiece(PIECE_e piece)
{
	setXpos(piece, (uint8_t)X(0));
	setYpos(piece, (uint8_t)Y(0));
	setZpos(piece, ZPOS_ALONE);
	setPieceHasBeenPlayed(piece, true);
	board[X(0)][Y(0)] = piece;
}


void movePiece(PIECE_e pieceToMove, PIECE_e adjacentPiece, DIRECTIONS_e adjacencyType)
{
	uint8_t xpos, ypos;
	
	if (hasPieceBeenPlayed(pieceToMove))
		pickUpPiece(pieceToMove);
	
	xpos = getXpos(adjacentPiece);
	ypos = getYpos(adjacentPiece);
	
	if (NORTHEAST == adjacencyType)
	{
		//xpos += 1;
		ypos += 1;
	}
	else if (EAST == adjacencyType)
	{
		xpos += 1;
	}
	else if (SOUTHEAST == adjacencyType)
	{
		xpos += 1;
		ypos -= 1;
	}
	else if (SOUTHWEST == adjacencyType)
	{
		//xpos -= 1;
		ypos -= 1;
	}
	else if (WEST == adjacencyType)
	{
		xpos -=1;
	}
	else if (NORTHWEST == adjacencyType)
	{
		xpos -= 1;
		ypos += 1;
	}
	
	putDownPiece(pieceToMove, xpos, ypos);
}

void playMove(MOVE_t *moves, uint32_t index)
{
	PIECE_e piece;
	uint8_t x, y;
	piece = ((moves[index].move & PIECE_MASK) >> PIECE_SHIFT);
	x = ((moves[index].move & XPOS_MASK) >> XPOS_SHIFT);
	y = ((moves[index].move & YPOS_MASK) >> YPOS_SHIFT);
	
	if (hasPieceBeenPlayed(piece))
		pickUpPiece(piece);
	
	putDownPiece(piece, x, y);
	moveNumber++;
#if ENABLE_TRANSPOSITION_TABLE
	addPieceToZobristKey(piece);
#endif
}


void undoMove(PIECE_t originalPiece, PIECE_e piece)
{
	uint8_t x, y;
	
	pickUpPiece(piece);
	if ((originalPiece.piece & BEEN_PLAYED_MASK) >> BEEN_PLAYED_SHIFT)
	{
		x = ((originalPiece.piece & XCOORD_MASK) >> XCOORD_SHIFT);
		y = ((originalPiece.piece & YCOORD_MASK) >> YCOORD_SHIFT);
		
		putDownPiece(piece, x, y);
	}
	moveNumber--;
#if ENABLE_TRANSPOSITION_TABLE
	addPieceToZobristKey(piece);
#endif
}


PIECE_e translateTypeAndNumberToIndex(char color, char type, char number)
{
	if ('w' == color)
	{
		if ('Q' == type)
		{
			return WHITE_QUEEN;
		}
		else if ('B' == type)
		{
			if ('1' == number)
			{
				return WHITE_BEETLE_ONE;
			}
			else if ('2' == number)
			{
				return WHITE_BEETLE_TWO;
			}
		}
		else if ('G' == type)
		{
			if ('1' == number)
			{
				return WHITE_GRASSHOPPER_ONE;
			}
			else if ('2' == number)
			{
				return WHITE_GRASSHOPPER_TWO;
			}
			else if ('3' == number)
			{
				return WHITE_GRASSHOPPER_THREE;
			}
		}
		else if ('A' == type)
		{
			if ('1' == number)
			{
				return WHITE_ANT_ONE;
			}
			else if ('2' == number)
			{
				return WHITE_ANT_TWO;
			}
			else if ('3' == number)
			{
				return WHITE_ANT_THREE;
			}
		}
		else if ('S' == type)
		{
			if ('1' == number)
			{
				return WHITE_SPIDER_ONE;
			}
			else if ('2' == number)
			{
				return WHITE_SPIDER_TWO;
			}
		}
	}
	else if ('b' == color)
	{
		if ('Q' == type)
		{
			return BLACK_QUEEN;
		}
		else if ('B' == type)
		{
			if ('1' == number)
			{
				return BLACK_BEETLE_ONE;
			}
			else if ('2' == number)
			{
				return BLACK_BEETLE_TWO;
			}
		}
		else if ('G' == type)
		{
			if ('1' == number)
			{
				return BLACK_GRASSHOPPER_ONE;
			}
			else if ('2' == number)
			{
				return BLACK_GRASSHOPPER_TWO;
			}
			else if ('3' == number)
			{
				return BLACK_GRASSHOPPER_THREE;
			}
		}
		else if ('A' == type)
		{
			if ('1' == number)
			{
				return BLACK_ANT_ONE;
			}
			else if ('2' == number)
			{
				return BLACK_ANT_TWO;
			}
			else if ('3' == number)
			{
				return BLACK_ANT_THREE;
			}
		}
		else if ('S' == type)
		{
			if ('1' == number)
			{
				return BLACK_SPIDER_ONE;
			}
			else if ('2' == number)
			{
				return BLACK_SPIDER_TWO;
			}
		}
	}
	return MAX_PIECES;
}

void initPieces(void)
{
	int i;
	for (i = 0; i < MAX_PIECES; i++)
	{
		pieces[i].piece = 0;
		if (i < BLACK_QUEEN)
		{
			setColor(i, WHITE);
		}
		else
		{
			setColor(i, BLACK);
		}
	}
}

inline void setXpos (PIECE_e piece, uint8_t xpos)
{
	// clear the old x position and then write the new position
    pieces[piece].piece &= ~XCOORD_MASK;
    pieces[piece].piece |= (xpos << XCOORD_SHIFT);
}

inline void setYpos (PIECE_e piece, uint8_t ypos)
{
	// clear the old x position and then write the new position
    pieces[piece].piece &= ~YCOORD_MASK;
    pieces[piece].piece |= (ypos << YCOORD_SHIFT);
}

inline void setZpos (PIECE_e piece, uint8_t zpos)
{
	// clear the old x position and then write the new position
    pieces[piece].piece &= ~ZCOORD_MASK;
    pieces[piece].piece |= (zpos << ZCOORD_SHIFT);
}


inline void setColor(PIECE_e piece, bool white)
{
	pieces[piece].piece &= ~COLOR_MASK;
	if (white)
	{
		pieces[piece].piece |= (1 << COLOR_SHIFT);
	}
	else
	{
		pieces[piece].piece |= (2 << COLOR_SHIFT);
	}
}


inline uint8_t getXpos (PIECE_e piece)
{
    return ((pieces[piece].piece & XCOORD_MASK) >> XCOORD_SHIFT);
}

inline uint8_t getYpos (PIECE_e piece)
{
    return ((pieces[piece].piece & YCOORD_MASK) >> YCOORD_SHIFT);
}

inline uint8_t getZpos (PIECE_e piece)
{
    return ((pieces[piece].piece & ZCOORD_MASK) >> ZCOORD_SHIFT);
}

inline bool isBeetle (PIECE_e piece)
{
    return ((piece == WHITE_BEETLE_ONE) ||
            (piece == WHITE_BEETLE_TWO) ||
            (piece == BLACK_BEETLE_ONE) ||
            (piece == BLACK_BEETLE_TWO));
}


inline bool isSpider (PIECE_e piece)
{
    return ((piece == WHITE_SPIDER_ONE) ||
            (piece == WHITE_SPIDER_TWO) ||
            (piece == BLACK_SPIDER_ONE) ||
            (piece == BLACK_SPIDER_TWO));
}

inline bool isQueen (PIECE_e piece)
{
    return ((piece == WHITE_QUEEN) ||
            (piece == BLACK_QUEEN));
}


 
inline bool isAnt (PIECE_e piece)
{
    return ((piece == WHITE_ANT_ONE) ||
            (piece == WHITE_ANT_TWO) ||
            (piece == WHITE_ANT_THREE) ||
            (piece == BLACK_ANT_ONE) ||
            (piece == BLACK_ANT_TWO) ||
            (piece == BLACK_ANT_THREE));
}

inline bool isGrasshopper (PIECE_e piece)
{
    return ((piece == WHITE_GRASSHOPPER_ONE) ||
            (piece == WHITE_GRASSHOPPER_TWO) ||
            (piece == WHITE_GRASSHOPPER_THREE) ||
            (piece == BLACK_GRASSHOPPER_ONE) ||
            (piece == BLACK_GRASSHOPPER_TWO) ||
            (piece == BLACK_GRASSHOPPER_THREE));
}

inline bool isWhite(PIECE_e piece)
{
	if (piece < BLACK_QUEEN)
		return true;
	return false;
}


inline bool hasPieceBeenPlayed(PIECE_e piece)
{
	return ((pieces[piece].piece & BEEN_PLAYED_MASK) >> BEEN_PLAYED_SHIFT);
}

inline void setPieceHasBeenPlayed(PIECE_e piece, bool placed)
{
	if (placed)
	{
		pieces[piece].piece |= (1 << BEEN_PLAYED_SHIFT);		
	}
	else
	{
		pieces[piece].piece &= ~(1 << BEEN_PLAYED_SHIFT);
	}
}


inline bool isStuckUnderABeetle(PIECE_e piece)
{
	if (board[getXpos(piece)][getYpos(piece)] != piece)
		return true;
	return false;
}

inline bool isAlmostSurrounded(PIECE_e piece)
{
	if (getNumberOfOccupiedSquaresAroundCoords(getXpos(piece), getYpos(piece)) == 5)
		return true;
	return false;
}

// Append the legal queen moves to moveList[] starting at index.
// Return the number of moves appended.
uint32_t getQueenMoves(MOVE_t* moves, PIECE_e piece, uint32_t index)
{
	uint8_t xpos, ypos, occupiedAdjacencies;
	MOVE_t currentMove;
	uint32_t currentIndex = index;
	
	
	if (!hasPieceBeenPlayed(piece))
	{
		// find and return valid starting places for the queen
		return getStartingPositions(moves, piece, index);
	}
	
	// only call this after making sure this piece has been played
	if (isStuckUnderABeetle(piece) || isAlmostSurrounded(piece))
	{
		return 0;
	}
	
	xpos = getXpos(piece);
	ypos = getYpos(piece);
	
	pickUpPiece(piece);
	if (isHiveBroken(piece))
	{
		putDownPiece(piece, xpos, ypos);
		return 0;		
	}

	occupiedAdjacencies = getOccupiedPositionsAroundCoords(xpos, ypos);
		
	
	if (!(occupiedAdjacencies & (1 << NE_SHIFT)))
	{
		if (isTouchingCommonPiece(xpos, ypos, NORTHEAST) && isSlidable(xpos, ypos, NORTHEAST))
		{
			currentMove.move = ((xpos << XPOS_SHIFT) | ((ypos + 1) << YPOS_SHIFT) | (piece << PIECE_SHIFT));
			moves[currentIndex++] = currentMove;
		}
	}
	
	if (!(occupiedAdjacencies & (1 << E_SHIFT)))
	{
		if (isTouchingCommonPiece(xpos, ypos, EAST) && isSlidable(xpos, ypos, EAST))
		{
			currentMove.move = (((xpos + 1) << XPOS_SHIFT) | (ypos << YPOS_SHIFT) | (piece << PIECE_SHIFT));
			moves[currentIndex++] = currentMove;
		}
	}	

	if (!(occupiedAdjacencies & (1 << SE_SHIFT)))
	{
		if (isTouchingCommonPiece(xpos, ypos, SOUTHEAST) && isSlidable(xpos, ypos, SOUTHEAST))
		{
			currentMove.move = (((xpos + 1) << XPOS_SHIFT) | ((ypos - 1) << YPOS_SHIFT) | (piece << PIECE_SHIFT));
			moves[currentIndex++] = currentMove;
		}
	}	
	
	if (!(occupiedAdjacencies & (1 << SW_SHIFT)))
	{
		if (isTouchingCommonPiece(xpos, ypos, SOUTHWEST) && isSlidable(xpos, ypos, SOUTHWEST))
		{
			currentMove.move = ((xpos << XPOS_SHIFT) | ((ypos - 1) << YPOS_SHIFT) | (piece << PIECE_SHIFT));
			moves[currentIndex++] = currentMove;
		}
	}
	
	if (!(occupiedAdjacencies & (1 << W_SHIFT)))
	{
		if (isTouchingCommonPiece(xpos, ypos, WEST) && isSlidable(xpos, ypos, WEST))
		{
			currentMove.move = (((xpos - 1) << XPOS_SHIFT) | (ypos << YPOS_SHIFT) | (piece << PIECE_SHIFT));
			moves[currentIndex++] = currentMove;
		}
	}		
	
	if (!(occupiedAdjacencies & (1 << NW_SHIFT)))
	{
		if (isTouchingCommonPiece(xpos, ypos, NORTHWEST) && isSlidable(xpos, ypos, NORTHWEST))
		{
			currentMove.move = (((xpos - 1) << XPOS_SHIFT) | ((ypos + 1) << YPOS_SHIFT) | (piece << PIECE_SHIFT));
			moves[currentIndex++] = currentMove;
		}	
	}
	
	putDownPiece(piece, xpos, ypos);
	
	return (currentIndex - index);
}

// queen has been played and i haven't - starting pos
// queen has been played and so have i - normal

// queen has not been played and i haven't
// if move < 4: starting pos 
// else if move == 4 return 0
// else return startingpos

// queen has not been played and i have - return 0;


// Append the legal beetle moves to moveList[] starting at index.
// Return the number of moves appended.
uint32_t getBeetleMoves(MOVE_t *moves, PIECE_e piece, uint32_t index)
{
	uint8_t xpos, ypos;
	MOVE_t currentMove;
	uint32_t currentIndex = index;
	PIECE_e myQueen;
	bool iHaveBeenPlayed;
	
	if (isWhite(piece))
		myQueen = WHITE_QUEEN;
	else
		myQueen = BLACK_QUEEN;

	iHaveBeenPlayed = hasPieceBeenPlayed(piece);
	
	if (!hasPieceBeenPlayed(myQueen))
	{
		// if my queen hasn't been played and it's move 4 (for this player),
		// the only legal moves for me are queen placement moves
		if (moveNumber == 8 || moveNumber == 7)
		{
			return 0;
		}
		else 
		{
			if (iHaveBeenPlayed)
			{
				// can't move me until the queen has been played
				return 0;
			}
			else 
			{
				return getStartingPositions(moves, piece, index);					
			}
		}
	}
	
	// if we're here, queen has been played
	// so if i haven't, give starting moves, otherwise give normal movement moves
	if (!hasPieceBeenPlayed(piece))
	{
		// find and return valid starting places for the beetle
		return getStartingPositions(moves, piece, index);
	}
	
	// only call this after making sure this piece has been played
	if (isStuckUnderABeetle(piece))
	{
		return 0;
	}
	
	xpos = getXpos(piece);
	ypos = getYpos(piece);
	
	pickUpPiece(piece);
	if (isHiveBroken(piece))
	{
		putDownPiece(piece, xpos, ypos);
		return 0;		
	}
	
	// Beetle can go anywhere as long as he has at least 1 neighbour

	// Beetle can either go on top of an adjacent piece or slide
	// along as long as the new position shares an adjacent piece
	// and isn't a gate.
	
	// NE
	if (isSquareOccupied(xpos, ypos+1) || (isTouchingCommonPiece(xpos, ypos, NORTHEAST) && isSlidable(xpos, ypos, NORTHEAST)))
	{
		currentMove.move = ((xpos << XPOS_SHIFT) | ((ypos + 1) << YPOS_SHIFT) | (piece << PIECE_SHIFT));
		moves[currentIndex++] = currentMove;
	}
	
	// E
	if (isSquareOccupied(xpos+1, ypos) || (isTouchingCommonPiece(xpos, ypos, EAST) && isSlidable(xpos, ypos, EAST)))
	{
		currentMove.move = (((xpos + 1) << XPOS_SHIFT) | (ypos << YPOS_SHIFT) | (piece << PIECE_SHIFT));
		moves[currentIndex++] = currentMove;
	}
		
	// SE
	if (isSquareOccupied(xpos+1, ypos-1) || (isTouchingCommonPiece(xpos, ypos, SOUTHEAST) && isSlidable(xpos, ypos, SOUTHEAST)))
	{
		currentMove.move = (((xpos + 1) << XPOS_SHIFT) | ((ypos - 1) << YPOS_SHIFT) | (piece << PIECE_SHIFT));
		moves[currentIndex++] = currentMove;
	}
	
	// SW
	if (isSquareOccupied(xpos, ypos-1) || (isTouchingCommonPiece(xpos, ypos, SOUTHWEST) && isSlidable(xpos, ypos, SOUTHWEST)))
	{
		currentMove.move = ((xpos << XPOS_SHIFT) | ((ypos - 1) << YPOS_SHIFT) | (piece << PIECE_SHIFT));
		moves[currentIndex++] = currentMove;	
	}
	
	// W
	if (isSquareOccupied(xpos-1, ypos) || (isTouchingCommonPiece(xpos, ypos, WEST) && isSlidable(xpos, ypos, WEST)))
	{
		currentMove.move = (((xpos - 1) << XPOS_SHIFT) | (ypos << YPOS_SHIFT) | (piece << PIECE_SHIFT));
		moves[currentIndex++] = currentMove;
	}
			

	// NW
	if (isSquareOccupied(xpos-1, ypos+1) || (isTouchingCommonPiece(xpos, ypos, NORTHWEST) && isSlidable(xpos, ypos, NORTHWEST)))
	{	
		currentMove.move = (((xpos - 1) << XPOS_SHIFT) | ((ypos + 1) << YPOS_SHIFT) | (piece << PIECE_SHIFT));
		moves[currentIndex++] = currentMove;
	}
	
	putDownPiece(piece, xpos, ypos);
	
	return (currentIndex - index);	
}

// Append the legal grasshopper moves to moveList[] starting at index.
// Return the number of moves appended.
uint32_t getGrasshopperMoves(MOVE_t *moves, PIECE_e piece, uint32_t index)
{
	uint8_t xpos, ypos, occupiedAdjacencies;
	uint8_t tempXpos, tempYpos;
	MOVE_t currentMove;
	uint32_t currentIndex = index;
	PIECE_e myQueen;
	bool iHaveBeenPlayed;
	
	if (isWhite(piece))
		myQueen = WHITE_QUEEN;
	else
		myQueen = BLACK_QUEEN;
	
	iHaveBeenPlayed = hasPieceBeenPlayed(piece);
	
	if (!hasPieceBeenPlayed(myQueen))
	{
		// if my queen hasn't been played and it's move 4 (for this player),
		// the only legal moves for me are queen placement moves
		if (moveNumber == 8 || moveNumber == 7)
		{
			return 0;
		}
		else 
		{
			if (iHaveBeenPlayed)
			{
				// can't move me until the queen has been played
				return 0;
			}
			else 
			{
				return getStartingPositions(moves, piece, index);					
			}
		}
	}
	
	// if we're here, queen has been played
	// so if i haven't, give starting moves, otherwise give normal movement moves
	if (!hasPieceBeenPlayed(piece))
	{
		// find and return valid starting places for the grasshopper
		return getStartingPositions(moves, piece, index);
	}
	
	// only call this after making sure this piece has been played
	if (isStuckUnderABeetle(piece))
	{
		return 0;
	}
	
	xpos = getXpos(piece);
	ypos = getYpos(piece);
	
	pickUpPiece(piece);
	if (isHiveBroken(piece))
	{
		putDownPiece(piece, xpos, ypos);
		return 0;		
	}
	
	// grasshopper must jump over at least one piece, so the only viable directions to move
	// are directions that are already occupied
	
	occupiedAdjacencies = getOccupiedPositionsAroundCoords(xpos, ypos);
	
	
	if (occupiedAdjacencies & (1 << NE_SHIFT))
	{
		tempXpos = xpos;
		tempYpos = ypos;
		// keep iterating in this direction until we find an empty space
		do 
		{
			tempYpos++;
		} while (isSquareOccupied(tempXpos, tempYpos));
		
		currentMove.move = ((tempXpos << XPOS_SHIFT) | (tempYpos << YPOS_SHIFT) | (piece << PIECE_SHIFT));
		moves[currentIndex++].move = currentMove.move;
	
	}
	
	if (occupiedAdjacencies & (1 << E_SHIFT))
	{
		tempXpos = xpos;
		tempYpos = ypos;
		// keep iterating in this direction until we find an empty space
		do 
		{
			tempXpos++;
		} while (isSquareOccupied(tempXpos, tempYpos));
		
		currentMove.move = ((tempXpos << XPOS_SHIFT) | (tempYpos << YPOS_SHIFT) | (piece << PIECE_SHIFT));
		moves[currentIndex++].move = currentMove.move;
	}	
	
	if (occupiedAdjacencies & (1 << SE_SHIFT))
	{
		tempXpos = xpos;
		tempYpos = ypos;
		// keep iterating in this direction until we find an empty space
		do 
		{
			tempXpos++;
			tempYpos--;
		} while (isSquareOccupied(tempXpos, tempYpos));
		
		currentMove.move = ((tempXpos << XPOS_SHIFT) | (tempYpos << YPOS_SHIFT) | (piece << PIECE_SHIFT));
		moves[currentIndex++].move = currentMove.move;
	}	
	
	if (occupiedAdjacencies & (1 << SW_SHIFT))
	{
		tempXpos = xpos;
		tempYpos = ypos;
		// keep iterating in this direction until we find an empty space
		do 
		{
			tempYpos--;
		} while (isSquareOccupied(tempXpos, tempYpos));
		
		currentMove.move = ((tempXpos << XPOS_SHIFT) | (tempYpos << YPOS_SHIFT) | (piece << PIECE_SHIFT));
		moves[currentIndex++].move = currentMove.move;
	}
	
	if (occupiedAdjacencies & (1 << W_SHIFT))
	{
		tempXpos = xpos;
		tempYpos = ypos;
		// keep iterating in this direction until we find an empty space
		do 
		{
			tempXpos--;
		} while (isSquareOccupied(tempXpos, tempYpos));
		
		currentMove.move = ((tempXpos << XPOS_SHIFT) | (tempYpos << YPOS_SHIFT) | (piece << PIECE_SHIFT));
		moves[currentIndex++].move = currentMove.move;
	}		
	
	if (occupiedAdjacencies & (1 << NW_SHIFT))
	{
		tempXpos = xpos;
		tempYpos = ypos;
		// keep iterating in this direction until we find an empty space
		do 
		{
			tempXpos--;
			tempYpos++;
		} while (isSquareOccupied(tempXpos, tempYpos));
		
		currentMove.move = ((tempXpos << XPOS_SHIFT) | (tempYpos << YPOS_SHIFT) | (piece << PIECE_SHIFT));
		moves[currentIndex++].move = currentMove.move;	
	}
	
	putDownPiece(piece, xpos, ypos);
	
	return (currentIndex - index);
}


// Append the legal ant moves to moveList[] starting at index.
// Return the number of moves appended.
uint32_t getAntMoves(MOVE_t *moves, PIECE_e piece, uint32_t index)
{
	uint8_t xpos, ypos;
	uint32_t movesFound = 0;
	PIECE_e myQueen;
	bool iHaveBeenPlayed;
	
	if (isWhite(piece))
		myQueen = WHITE_QUEEN;
	else
		myQueen = BLACK_QUEEN;
	
	iHaveBeenPlayed = hasPieceBeenPlayed(piece);
	
	if (!hasPieceBeenPlayed(myQueen))
	{
		// if my queen hasn't been played and it's move 4 (for this player),
		// the only legal moves for me are queen placement moves
		if (moveNumber == 8 || moveNumber == 7)
		{
			return 0;
		}
		else 
		{
			if (iHaveBeenPlayed)
			{
				// can't move me until the queen has been played
				return 0;
			}
			else 
			{
				return getStartingPositions(moves, piece, index);					
			}
		}
	}
	
	// if we're here, queen has been played
	// so if i haven't, give starting moves, otherwise give normal movement moves
	if (!hasPieceBeenPlayed(piece))
	{
		// find and return valid starting places for the ant
		return getStartingPositions(moves, piece, index);
	}
	
	// only call this after making sure this piece has been played
	if (isStuckUnderABeetle(piece) || isAlmostSurrounded(piece))
	{
		return 0;
	}
	
	xpos = getXpos(piece);
	ypos = getYpos(piece);
	
	pickUpPiece(piece);
	if (isHiveBroken(piece))
	{
		putDownPiece(piece, xpos, ypos);
		return 0;		
	}
	
	// Check surrounding positions for a valid start spot for the antSearch:
	
	// NORTHEAST
	if (!isGateOrUnnattached(xpos, ypos+1) && !isSquareOccupied(xpos, ypos+1) && isSlidable(xpos, ypos, NORTHEAST))
	{
		movesFound = antSearch(moves, piece, index, xpos, ypos+1, index);
	}
	// EAST
	else if (!isGateOrUnnattached(xpos+1, ypos) && !isSquareOccupied(xpos+1, ypos) && isSlidable(xpos, ypos, EAST))
	{
		movesFound = antSearch(moves, piece, index, xpos+1, ypos, index);
	}
	
	// SOUTHEAST
	else if (!isGateOrUnnattached(xpos+1, ypos-1) && !isSquareOccupied(xpos+1, ypos-1) && isSlidable(xpos, ypos, SOUTHEAST))
	{
		movesFound = antSearch(moves, piece, index, xpos+1, ypos-1, index);
	}
	
	// SOUTHWEST
	else if (!isGateOrUnnattached(xpos, ypos-1) && !isSquareOccupied(xpos, ypos-1) && isSlidable(xpos, ypos, SOUTHWEST))
	{
		movesFound = antSearch(moves, piece, index, xpos, ypos-1, index);
	}
	
	// WEST
	else if (!isGateOrUnnattached(xpos-1, ypos) && !isSquareOccupied(xpos-1, ypos) && isSlidable(xpos, ypos, WEST))
	{
		movesFound = antSearch(moves, piece, index, xpos-1, ypos, index);
	}
	
	// NORTHWEST
	else if (!isGateOrUnnattached(xpos-1, ypos+1) && !isSquareOccupied(xpos-1, ypos+1) && isSlidable(xpos, ypos, NORTHWEST))
	{
		movesFound = antSearch(moves, piece, index, xpos-1, ypos+1, index);
	}
	
	putDownPiece(piece, xpos, ypos);
	return movesFound;
}


// Append the legal spider moves to moveList[] starting at index.
// Return the number of moves appended.
uint32_t getSpiderMoves(MOVE_t *moves, PIECE_e piece, uint32_t index)
{
	uint8_t xpos, ypos;
	uint32_t movesFound = 0;
	PIECE_e myQueen;
	bool iHaveBeenPlayed;
	
	if (isWhite(piece))
		myQueen = WHITE_QUEEN;
	else
		myQueen = BLACK_QUEEN;
	
	iHaveBeenPlayed = hasPieceBeenPlayed(piece);
	
	if (!hasPieceBeenPlayed(myQueen))
	{
		// if my queen hasn't been played and it's move 4 (for this player),
		// the only legal moves for me are queen placement moves
		if (moveNumber == 8 || moveNumber == 7)
		{
			return 0;
		}
		else 
		{
			if (iHaveBeenPlayed)
			{
				// can't move me until the queen has been played
				return 0;
			}
			else 
			{
				return getStartingPositions(moves, piece, index);					
			}
		}
	}
	
	// if we're here, queen has been played
	// so if i haven't, give starting moves, otherwise give normal movement moves
	if (!hasPieceBeenPlayed(piece))
	{
		// find and return valid starting places for the ant
		return getStartingPositions(moves, piece, index);
	}
	
	// only call this after making sure this piece has been played
	if (isStuckUnderABeetle(piece) || isAlmostSurrounded(piece))
	{
		return 0;
	}
	
	xpos = getXpos(piece);
	ypos = getYpos(piece);
	
	pickUpPiece(piece);
	if (isHiveBroken(piece))
	{
		putDownPiece(piece, xpos, ypos);
		return 0;		
	}
	
	// Check surrounding positions and start spiderSearch:
	
	// NORTHEAST
	if (!isGateOrUnnattached(xpos, ypos+1) && !isSquareOccupied(xpos, ypos+1) && isTouchingCommonPiece(xpos, ypos, NORTHEAST) && isSlidable(xpos, ypos, NORTHEAST))
	{
		movesFound += spiderSearch(moves, piece, index, xpos, ypos+1, index, 3, xpos, ypos);
	}
	
	// EAST
	if (!isGateOrUnnattached(xpos+1, ypos) && !isSquareOccupied(xpos+1, ypos) && isTouchingCommonPiece(xpos, ypos, EAST) && isSlidable(xpos, ypos, EAST))
	{
		movesFound += spiderSearch(moves, piece, index, xpos+1, ypos, index+movesFound, 3, xpos, ypos);
	}
	
	// SOUTHEAST
	if (!isGateOrUnnattached(xpos+1, ypos-1) && !isSquareOccupied(xpos+1, ypos-1)  && isTouchingCommonPiece(xpos, ypos, SOUTHEAST) && isSlidable(xpos, ypos, SOUTHEAST))
	{
		movesFound += spiderSearch(moves, piece, index, xpos+1, ypos-1, index+movesFound, 3, xpos, ypos);
	}
	
	// SOUTHWEST
	if (!isGateOrUnnattached(xpos, ypos-1) && !isSquareOccupied(xpos, ypos-1)  && isTouchingCommonPiece(xpos, ypos, SOUTHWEST) && isSlidable(xpos, ypos, SOUTHWEST))
	{
		movesFound += spiderSearch(moves, piece, index, xpos, ypos-1, index+movesFound, 3, xpos, ypos);
	}
	
	// WEST
	if (!isGateOrUnnattached(xpos-1, ypos) && !isSquareOccupied(xpos-1, ypos)  && isTouchingCommonPiece(xpos, ypos, WEST) && isSlidable(xpos, ypos, WEST))
	{
		movesFound += spiderSearch(moves, piece, index, xpos-1, ypos, index+movesFound, 3, xpos, ypos);
	}
	
	// NORTHWEST
	if (!isGateOrUnnattached(xpos-1, ypos+1) && !isSquareOccupied(xpos-1, ypos+1)  && isTouchingCommonPiece(xpos, ypos, NORTHWEST) && isSlidable(xpos, ypos, NORTHWEST))
	{
		movesFound += spiderSearch(moves, piece, index, xpos-1, ypos+1, index+movesFound, 3, xpos, ypos);
	}
	
	putDownPiece(piece, xpos, ypos);
	return movesFound;	
}


// add all legal starting positions for piece to moves[startIndex]
// return the number of moves added
uint32_t getStartingPositions(MOVE_t *moves, PIECE_e piece, uint32_t startIndex)
{
	PIECE_e i;
	bool white = isWhite(piece);
	uint32_t positionsFound = 0;
	
	for (i = 0; i < MAX_PIECES; i++)
	{
		if (hasPieceBeenPlayed(i) && (isWhite(i) == white))
		{
			// find the possible deployment squares around this piece
			positionsFound += startPositionSearch(moves, i, piece, startIndex, startIndex + positionsFound);
		}
	}

	return positionsFound;
}

// Search the squares around this piece. Add any legal starting positions that aren't already
// on the moves list
static uint32_t startPositionSearch(MOVE_t *moves, PIECE_e piece, PIECE_e pieceToPlay, uint32_t startIndex, uint32_t currentIndex)
{
	bool white;
	uint8_t x, y;
	MOVE_t currentMove;
	uint32_t movesAdded = 0;
	
	white = isWhite(piece);
	x = getXpos(piece);
	y = getYpos(piece);
	
	
	// NE
	if (board[xNE(x)][yNE(y)] == EMPTY_SPACE)
	{
		if (isTouchingOnlySameColor(white, xNE(x), yNE(y)))
		{
			currentMove.move = ((xNE(x) << XPOS_SHIFT) | (yNE(y) << YPOS_SHIFT) | (pieceToPlay << PIECE_SHIFT));
			if (!isMoveAlreadyOnMoveList(moves, startIndex, currentIndex + movesAdded, currentMove))
			{
				moves[currentIndex + movesAdded++].move = currentMove.move;				
			}
		}
	}
	
	// E
	if (board[xE(x)][yE(y)] == EMPTY_SPACE)
	{
		if (isTouchingOnlySameColor(white, xE(x), yE(y)))
		{
			currentMove.move = ((xE(x) << XPOS_SHIFT) | (yE(y) << YPOS_SHIFT) | (pieceToPlay << PIECE_SHIFT));
			if (!isMoveAlreadyOnMoveList(moves, startIndex, currentIndex + movesAdded, currentMove))
			{
				moves[currentIndex + movesAdded++].move = currentMove.move;				
			}
		}
	}
	
	// SE
	if (board[xSE(x)][ySE(y)] == EMPTY_SPACE)
	{
		if (isTouchingOnlySameColor(white, xSE(x), ySE(y)))
		{
			currentMove.move = ((xSE(x) << XPOS_SHIFT) | (ySE(y) << YPOS_SHIFT) | (pieceToPlay << PIECE_SHIFT));
			if (!isMoveAlreadyOnMoveList(moves, startIndex, currentIndex + movesAdded, currentMove))
			{
				moves[currentIndex + movesAdded++].move = currentMove.move;				
			}
		}
	}
	
	// SW
	if (board[xSW(x)][ySW(y)] == EMPTY_SPACE)
	{
		if (isTouchingOnlySameColor(white, xSW(x), ySW(y)))
		{
			currentMove.move = ((xSW(x) << XPOS_SHIFT) | (ySW(y) << YPOS_SHIFT) | (pieceToPlay << PIECE_SHIFT));
			if (!isMoveAlreadyOnMoveList(moves, startIndex, currentIndex + movesAdded, currentMove))
			{
				moves[currentIndex + movesAdded++].move = currentMove.move;				
			}
		}
	}
	
	// W
	if (board[xW(x)][yW(y)] == EMPTY_SPACE)
	{
		if (isTouchingOnlySameColor(white, xW(x), yW(y)))
		{
			currentMove.move = ((xW(x) << XPOS_SHIFT) | (yW(y) << YPOS_SHIFT) | (pieceToPlay << PIECE_SHIFT));
			if (!isMoveAlreadyOnMoveList(moves, startIndex, currentIndex + movesAdded, currentMove))
			{
				moves[currentIndex + movesAdded++].move = currentMove.move;				
			}
		}
	}
	
	// NW
	if (board[xNW(x)][yNW(y)] == EMPTY_SPACE)
	{
		if (isTouchingOnlySameColor(white, xNW(x), yNW(y)))
		{
			currentMove.move = ((xNW(x) << XPOS_SHIFT) | (yNW(y) << YPOS_SHIFT) | (pieceToPlay << PIECE_SHIFT));
			if (!isMoveAlreadyOnMoveList(moves, startIndex, currentIndex + movesAdded, currentMove))
			{
				moves[currentIndex + movesAdded++].move = currentMove.move;				
			}
		}
	}
	
	return movesAdded;
}

static uint32_t spiderSearch(MOVE_t *moves, PIECE_e piece, uint32_t startIndex, uint8_t x, uint8_t y, uint32_t index, uint32_t depth, uint8_t prevX, uint8_t prevY)
{
	MOVE_t currentMove;
	uint32_t movesAdded = 0;
	
	currentMove.move = ((x << XPOS_SHIFT) | (y << YPOS_SHIFT) | (piece << PIECE_SHIFT));
	
	// Validate that the current position is not already on the move list
	if (!isMoveAlreadyOnMoveList(moves, startIndex, index, currentMove) && !(x == getXpos(piece) && y == getYpos(piece)))
	{
		depth--;
		if (depth == 0)
		{
			moves[index + movesAdded++].move = currentMove.move;
		}
		else 
		{
			// Check surrounding positions:
			
			// NORTHEAST
			if (!isGateOrUnnattached(x, y+1) && !isSquareOccupied(x, y+1) && !(x == prevX && y+1 == prevY) && isTouchingCommonPiece(x, y, NORTHEAST) && isSlidable(x, y, NORTHEAST))
			{
				movesAdded += spiderSearch(moves, piece, startIndex, x, y+1, index+movesAdded, depth, x, y);
			}
		
			// EAST
			if (!isGateOrUnnattached(x+1, y)  && !isSquareOccupied(x+1, y) && !(x+1 == prevX && y == prevY) && isTouchingCommonPiece(x, y, EAST) && isSlidable(x, y, EAST))
			{
				movesAdded += spiderSearch(moves, piece, startIndex, x+1, y, index+movesAdded, depth, x, y);
			}
			
			// SOUTHEAST
			if (!isGateOrUnnattached(x+1, y-1)  && !isSquareOccupied(x+1, y-1) && !(x+1 == prevX && y-1 == prevY) && isTouchingCommonPiece(x, y, SOUTHEAST) && isSlidable(x, y, SOUTHEAST))
			{
				movesAdded += spiderSearch(moves, piece, startIndex, x+1, y-1, index+movesAdded, depth, x, y);	
			}
			
			// SOUTHWEST
			if (!isGateOrUnnattached(x, y-1) && !isSquareOccupied(x, y-1)  && !(x == prevX && y-1 == prevY) && isTouchingCommonPiece(x, y, SOUTHWEST) && isSlidable(x, y, SOUTHWEST))
			{
				movesAdded += spiderSearch(moves, piece, startIndex, x, y-1, index+movesAdded, depth, x, y);	
			}
			
			// WEST
			if (!isGateOrUnnattached(x-1, y) && !isSquareOccupied(x-1, y) && !(x-1 == prevX && y == prevY) && isTouchingCommonPiece(x, y, WEST) && isSlidable(x, y, WEST))
			{
				movesAdded += spiderSearch(moves, piece, startIndex, x-1, y, index+movesAdded, depth, x, y);	
			}
			
			// NORTHWEST
			if (!isGateOrUnnattached(x-1, y+1) && !isSquareOccupied(x-1, y+1)  && !(x-1 == prevX && y+1 == prevY) && isTouchingCommonPiece(x, y, NORTHWEST) && isSlidable(x, y, NORTHWEST))
			{
				movesAdded += spiderSearch(moves, piece, startIndex, x-1, y+1, index+movesAdded, depth, x, y);	
			}
		}
	}
	
	return movesAdded;
}


static uint32_t antSearch(MOVE_t *moves, PIECE_e piece, uint32_t startIndex, uint8_t x, uint8_t y, uint32_t index)
{
	MOVE_t currentMove;
	uint32_t movesAdded = 0;
	
	// If we are back to the starting position, return
	if (x == getXpos(piece) && y == getYpos(piece))
	{
		return movesAdded;
	}
	
	// Make sure we aren't going backwards
	if (index > startIndex)
	{
		if (isPreviousMoveCoords(moves, index, x, y))
		{
			return movesAdded;			
		}
	}
	
	currentMove.move = ((x << XPOS_SHIFT) | (y << YPOS_SHIFT) | (piece << PIECE_SHIFT));
	
	// Validate that the current position is not already on the move list
	if (!isMoveAlreadyOnMoveList(moves, startIndex, (index == 0 ? index : index - 1), currentMove)) // pass in index-1 because we already checked the most recent move above
	{
		// if it's good, add it to list
		moves[index + movesAdded++].move = currentMove.move;
		
		// Check surrounding positions:
		
		// NORTHEAST
		if (!isGateOrUnnattached(x, y+1) && !isSquareOccupied(x, y+1) && !isPreviousMoveCoords(moves, index+movesAdded, x, y+1) && isSlidable(x, y, NORTHEAST))
		{
			movesAdded += antSearch(moves, piece, startIndex, x, y+1, index + movesAdded);
		}
		
		// EAST
		if (!isGateOrUnnattached(x+1, y)  && !isSquareOccupied(x+1, y) && !isPreviousMoveCoords(moves, index+movesAdded, x+1, y) && isSlidable(x, y, EAST))
		{
			movesAdded += antSearch(moves, piece, startIndex, x+1, y, index + movesAdded);		
		}
		
		// SOUTHEAST
		if (!isGateOrUnnattached(x+1, y-1)  && !isSquareOccupied(x+1, y-1) && !isPreviousMoveCoords(moves, index+movesAdded, x+1, y-1) && isSlidable(x, y, SOUTHEAST))
		{
			movesAdded += antSearch(moves, piece, startIndex, x+1, y-1, index + movesAdded);		
		}
		
		// SOUTHWEST
		if (!isGateOrUnnattached(x, y-1) && !isSquareOccupied(x, y-1)  && !isPreviousMoveCoords(moves, index+movesAdded, x, y-1) && isSlidable(x, y, SOUTHWEST))
		{
			movesAdded += antSearch(moves, piece, startIndex, x, y-1, index + movesAdded);		
		}
		
		// WEST
		if (!isGateOrUnnattached(x-1, y) && !isSquareOccupied(x-1, y) && !isPreviousMoveCoords(moves, index+movesAdded, x-1, y) && isSlidable(x, y, WEST))
		{
			movesAdded += antSearch(moves, piece, startIndex, x-1, y, index + movesAdded);		
		}
		
		// NORTHWEST
		if (!isGateOrUnnattached(x-1, y+1) && !isSquareOccupied(x-1, y+1)  && !isPreviousMoveCoords(moves, index+movesAdded, x-1, y+1) && isSlidable(x, y, NORTHWEST))
		{
			movesAdded += antSearch(moves, piece, startIndex, x-1, y+1, index + movesAdded);		
		}
	}
	
	return movesAdded;
}


// add all legal moves to moves[] 
// return the number of moves added
uint32_t AddAllLegalMovesToMoveList(MOVE_t *moves, EVALUATE_INFO_t *eval, bool whiteToPlay)
{
	uint32_t movesFound = 0;
	uint32_t totalMovesFound = 0;
	PIECE_e i;
	
	if (whiteToPlay)
	{
		for (i = WHITE_QUEEN; i < BLACK_QUEEN; i++)
		{
			if (isQueen(i))
			{
				movesFound = getQueenMoves(moves, i, 0);
				eval->whiteMoves += movesFound;
				totalMovesFound += movesFound;
			}
			else if (isBeetle(i))
			{
				movesFound = getBeetleMoves(moves, i, totalMovesFound);
				eval->whiteMoves += movesFound;
				totalMovesFound += movesFound;
			}
			else if (isGrasshopper(i))
			{
				movesFound = getGrasshopperMoves(moves, i, totalMovesFound);
				eval->whiteMoves += movesFound;
				totalMovesFound += movesFound;
			}
			else if (isAnt(i))
			{
				movesFound = getAntMoves(moves, i, totalMovesFound);
				eval->whiteMoves += movesFound;
				totalMovesFound += movesFound;
			}
			else if (isSpider(i))
			{
				movesFound = getSpiderMoves(moves, i, totalMovesFound);
				eval->whiteMoves += movesFound;
				totalMovesFound += movesFound;
			}
			
			eval->validMoves[i] += movesFound;
		}
	}
	
	else 
	{
		for (i = BLACK_QUEEN; i < MAX_PIECES; i++)
		{
			if (isQueen(i))
			{
				movesFound = getQueenMoves(moves, i, totalMovesFound);
				eval->blackMoves += movesFound;
				totalMovesFound += movesFound;
			}
			else if (isBeetle(i))
			{
				movesFound = getBeetleMoves(moves, i, totalMovesFound);
				eval->blackMoves += movesFound;
				totalMovesFound += movesFound;
			}
			else if (isGrasshopper(i))
			{
				movesFound = getGrasshopperMoves(moves, i, totalMovesFound);
				eval->blackMoves += movesFound;
				totalMovesFound += movesFound;
			}
			else if (isAnt(i))
			{
				movesFound = getAntMoves(moves, i, totalMovesFound);
				eval->blackMoves += movesFound;
				totalMovesFound += movesFound;
			}
			else if (isSpider(i))
			{
				movesFound = getSpiderMoves(moves, i, totalMovesFound);
				eval->blackMoves += movesFound;
				totalMovesFound += movesFound;
			}
			eval->validMoves[i] += movesFound;
		}
	}
	
	if (moveNumber == 2)
	{
		movesFound = getMoveTwoMoves(moves);
		totalMovesFound = movesFound;
	}
	
	return totalMovesFound;
}

static uint32_t getMoveTwoMoves(MOVE_t *moves)
{
	// it's always black to play on move 2
	PIECE_e i;
	uint8_t x = 0;
	uint8_t y = 0; 
	uint32_t movesAdded = 0;
	MOVE_t currentMove;
	for (i = BLACK_QUEEN; i < MAX_PIECES; i++)
	{
		// manually add all possible moves around the start square
		
		// NE
		currentMove.move = ((xNE(X(x)) << XPOS_SHIFT) | (yNE(Y(y)) << YPOS_SHIFT) | (i << PIECE_SHIFT));
		moves[movesAdded++].move = currentMove.move;
		
		// E
		currentMove.move = ((xE(X(x)) << XPOS_SHIFT) | (yE(Y(y)) << YPOS_SHIFT) | (i << PIECE_SHIFT));
		moves[movesAdded++].move = currentMove.move;
		
		// SE
		currentMove.move = ((xSE(X(x)) << XPOS_SHIFT) | (ySE(Y(y)) << YPOS_SHIFT) | (i << PIECE_SHIFT));
		moves[movesAdded++].move = currentMove.move;
		
		// SW
		currentMove.move = ((xSW(X(x)) << XPOS_SHIFT) | (ySW(Y(y)) << YPOS_SHIFT) | (i << PIECE_SHIFT));
		moves[movesAdded++].move = currentMove.move;
		
		// W
		currentMove.move = ((xW(X(x)) << XPOS_SHIFT) | (yW(Y(y)) << YPOS_SHIFT) | (i << PIECE_SHIFT));
		moves[movesAdded++].move = currentMove.move;
		
		// NW
		currentMove.move = ((xNW(X(x)) << XPOS_SHIFT) | (yNW(Y(y)) << YPOS_SHIFT) | (i << PIECE_SHIFT));
		moves[movesAdded++].move = currentMove.move;
		
	}
	return movesAdded;
}

static inline bool isMoveAlreadyOnMoveList(MOVE_t *moves, uint32_t startIndex, uint32_t index, MOVE_t move)
{
	// iterate through the move list from startIndex to index
	// return true if we find the move, false otherwise
	uint32_t i;
	
	if (index <= startIndex)
	{
		return false;
	}
	
	for (i = startIndex; i < index; i++)
	{
		if (moves[i].move == move.move)
			return true;
	}
	return false;
}


static inline bool isPreviousMoveCoords(MOVE_t *moves, uint32_t index, uint8_t xpos, uint8_t ypos)
{
	uint8_t prevXpos, prevYpos;
	prevXpos = (moves[index-1].move & XPOS_MASK) >> XPOS_SHIFT;
	prevYpos = (moves[index-1].move & YPOS_MASK) >> YPOS_SHIFT;
	
	if (prevXpos == xpos && prevYpos == ypos)
	{
		return true;
	}
	return false;
}
								 

void initMoves(MOVE_t *moves)
{
	int i;
	for (i = 0; i < MOVE_LIST_SIZE; i++)
	{
		moves[i].move = 0;
	}
}
