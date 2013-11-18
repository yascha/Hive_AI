
#include "evaluate.h"

int evaluateBoard(EVALUATE_INFO_t *eval)
{
	int score = 0;
	
	uint8_t x, y, occupied;
	PIECE_e i;
	
	int whiteMovesForPlayedPieces = 0;
	int blackMovesForPlayedPieces = 0;
	
	// Add scores for pinned pieces
	for (i = WHITE_QUEEN; i < MAX_PIECES; i++)
	{
		if (hasPieceBeenPlayed(i))
		{
			if (eval->validMoves[i] == 0)
			{
				score += pinnedMap[i];				
			}
			if (i < BLACK_QUEEN)
			{
				whiteMovesForPlayedPieces += eval->validMoves[i];
			}
			else 
			{
				blackMovesForPlayedPieces += eval->validMoves[i];
			}
		}
	}
	
	// Add scores for number of mobile & deployed pieces
	score += (whiteMovesForPlayedPieces - blackMovesForPlayedPieces) * 5;
	
	
	// Add scores for surrounded queens
	if (hasPieceBeenPlayed(WHITE_QUEEN))
	{
		x = getXpos(WHITE_QUEEN);
		y = getYpos(WHITE_QUEEN);
		occupied = getNumberOfOccupiedSquaresAroundCoords(x, y);
		
		if (occupied == 5)
		{
			score -= FIVE_SQUARES_AROUND_QUEEN_SCORE;
		}
		else if (occupied == 4)
		{
			score -= FOUR_SQUARES_AROUND_QUEEN_SCORE;
		}
		else if (occupied == 3)
		{
			score -= THREE_SQUARES_AROUND_QUEEN_SCORE;
		}
		else if (occupied == 2)
		{
			score -= TWO_SQUARES_AROUND_QUEEN_SCORE;
		}
	}

	if (hasPieceBeenPlayed(BLACK_QUEEN))
	{
		x = getXpos(BLACK_QUEEN);
		y = getYpos(BLACK_QUEEN);
		occupied = getNumberOfOccupiedSquaresAroundCoords(x, y);
		
		if (occupied == 5)
		{
			score += FIVE_SQUARES_AROUND_QUEEN_SCORE;
		}
		else if (occupied == 4)
		{
			score += FOUR_SQUARES_AROUND_QUEEN_SCORE;
		}
		else if (occupied == 3)
		{
			score += THREE_SQUARES_AROUND_QUEEN_SCORE;
		}
		else if (occupied == 2)
		{
			score += TWO_SQUARES_AROUND_QUEEN_SCORE;
		}		
	}
		
	
	return score;	
}


void initPinnedMap(void)
{
	pinnedMap[WHITE_QUEEN] = -QUEEN_PINNED_SCORE;
	pinnedMap[WHITE_BEETLE_ONE] = -BEETLE_PINNED_SCORE;
	pinnedMap[WHITE_BEETLE_TWO] = -BEETLE_PINNED_SCORE;
	pinnedMap[WHITE_GRASSHOPPER_ONE] = -GRASSHOPPER_PINNED_SCORE;
	pinnedMap[WHITE_GRASSHOPPER_TWO] = -GRASSHOPPER_PINNED_SCORE;
	pinnedMap[WHITE_GRASSHOPPER_THREE] = -GRASSHOPPER_PINNED_SCORE;
	pinnedMap[WHITE_ANT_ONE] = -ANT_PINNED_SCORE;
	pinnedMap[WHITE_ANT_TWO] = -ANT_PINNED_SCORE;
	pinnedMap[WHITE_ANT_TWO] = -ANT_PINNED_SCORE;
	pinnedMap[WHITE_SPIDER_ONE] = -SPIDER_PINNED_SCORE;
	pinnedMap[WHITE_SPIDER_TWO] = -SPIDER_PINNED_SCORE;
	
	pinnedMap[BLACK_QUEEN] = QUEEN_PINNED_SCORE;
	pinnedMap[BLACK_BEETLE_ONE] = BEETLE_PINNED_SCORE;
	pinnedMap[BLACK_BEETLE_TWO] = BEETLE_PINNED_SCORE;
	pinnedMap[BLACK_GRASSHOPPER_ONE] = GRASSHOPPER_PINNED_SCORE;
	pinnedMap[BLACK_GRASSHOPPER_TWO] = GRASSHOPPER_PINNED_SCORE;
	pinnedMap[BLACK_GRASSHOPPER_THREE] = GRASSHOPPER_PINNED_SCORE;
	pinnedMap[BLACK_ANT_ONE] = ANT_PINNED_SCORE;
	pinnedMap[BLACK_ANT_TWO] = ANT_PINNED_SCORE;
	pinnedMap[BLACK_ANT_TWO] = ANT_PINNED_SCORE;
	pinnedMap[BLACK_SPIDER_ONE] = SPIDER_PINNED_SCORE;
	pinnedMap[BLACK_SPIDER_TWO] = SPIDER_PINNED_SCORE;
}



/* THOUGHT: USE ARRAYS TO STORE VALUES INSTEAD OF HAVING TO DO MULTIPLE IF STATEMENTS */

/*
 previously we had the idea of a static evaluate the board and come up with a position value 
 based on potential wins.
 this didn't depend on what the possible moves were or the mobility of the pieces.
 so there used to be a separation between what the valid moves are/were and what the score
 for a specific position evaluates to.
 
 i am proposing using the available moves/mobility as a means of determining the score for the board
 (among other things, such as queen mobility)
 
 this would associate the two
 
 is this a good idea?
 

 
 */


/* heuristic ideas:
 
 first check iswin:
 iswin -> +10000 or -10000
 
 if no win, evaluate position:
 
 - mobility of pieces
 - queen moves available  - # squares surrounding queen (let's call these almost-wins) 
 (also consider the number of my pieces surrounding their queen (this is a weighted almost win)
 - beetle moves available - this will be either 6 or 0; let's count 0 as pretty strong because it's good to trap a beetle
 - grasshopper moves available
 - ant moves available (somehow cap this or make it worth little per move)
 - spider moves available (max 2)
 
 - evaluate
 
 
 - the pieces should be weighted in value:
 - queen
 - beetle
 - ant
 - grasshopper
 - spider
 
 
 
 stuff to do preceding the heuristic:
 generate legal moves
 do it piece by piece
 queen
 beetle
 grasshopper
 ant
 spider
 
 for later; sort the moves in order of importance?
 
 for now:
 assume max number of moves per piece and malloc accordingly (what's the max moves for an ant?)
 
 generate moves for a queen:
 pick up - breakshive? 0 moves
 get all non-occupied coordinates around the queen
 iterate through them and check if it is a gate
 
 generate moves for a beetle:
 pick up - breaks hive? 0 moves
 get all coordinates around the location that are adjacent to (or on) another piece
 
 generate moves for a grasshopper:
 pick up - breaks hive? 0 moves
 get all coordinates around grasshopper that are occupied
 each of those will map directly to a move, just need to see how far the line of sight of pieces is
 
 generate moves for an ant:
 pick up - breaks hive? 0 moves
 find first valid move
 validate move before calling antSearch?
 what checks do i need to do?
 - not in current list
  - not previous location (hopefully don't even call antSearch with previous location)
 - not start square
 - not a gate
 - has adjoining pieces
 
 
 
 what are my options:
 1. do all the checks before calling antsearch and then antsearch just adds it and does all the checsk for surrounding squares
 pros:
 
 cons:
 end up doing a lot of checks on the same square multiple times (everytime we pass it)
 hard to debug
 
 2. just do isNotGate and hasAdjoiningPieces before calling antsearch
 pros:
 cons:
 
 
 generate moves for a spider:
 pick up - breaks hive? 0 moves
 
 
 basic search stuff:
 if white
 foreach white piece
 for each possible move
 make move
 check isWin
 if win, return eval
 else decrement depth, get all possible moves, repeat
 
 
 alpha-beta pruning algorithm
 
 
 def max-value(state, a, b, depth):
 if (depth == 0): return value(state)
 for s in SUCCESSORS(state):
 a = max(a, min-value(s,a,b,depth-1))
 if a >= b: return a \\ this ia a cutoff point
 return a
 def min-value(state, a, b, depth):
 if (depth == 0): return value(state)
 for s in SUCCESSORS(state):
 b = min(b,max-value(s,a,b,depth-1))
 if b <= a: return b \\ this is a cutoff point
 return 
 
 
 
 
 
 
 
 min-max algorithm
 
 def max-value(state,depth):
 if (depth == 0): return value(state)
 v = -infinite
 for each s in SUCCESSORS(state):
 v = MAX(v,min-value(s,depth-1))
 return v
 def min-value(state,depth):
 if (depth == 0): return value(state)
 v = infinite
 for each s in SUCCESSORS(state):
 v = MIN(v,max-value(s,depth-1))
 return 
 
 */