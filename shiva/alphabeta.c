

#include "alphabeta.h"

void calculateBestMove(bool whiteToPlay)
{
	int timeTakenSearching;
	currentDepth = FIXED_DEPTH;
	
	startTimer();
	alphaBeta(currentDepth, -INFINITY, INFINITY, whiteToPlay);
	timeTakenSearching = getSearchTime();
	//DEBUG("Move number = %d\n", moveNumber);
	DEBUG("Number of nodes searched = %d\n", numberNodes);
	DEBUG("Number of evaluations = %d\n", numberEvaluations);
	DEBUG("Time taken = %dms\n", timeTakenSearching);
	DEBUG("Position score = %d\n", currentScore);
	//DEBUG("Best move:\n\tpiece = %d\n\txpos = %d\n\typos = %d\n", 
//		  ((currMove.move & PIECE_MASK) >> PIECE_SHIFT),
//		  ((currMove.move & XPOS_MASK) >> XPOS_SHIFT),
//	((currMove.move & YPOS_MASK) >> YPOS_SHIFT));
#if ENABLE_TRANSPOSITION_TABLE
	DEBUG("Cache hits = %u\n", cacheHits);
	DEBUG("Cache misses = %u\n", cacheMisses);
#endif // ENABLE_TRANSPOSITION_TABLE

}



int alphaBeta(int depth, int alpha, int beta, bool whiteToPlay)
{
	int val, i;
	EVALUATE_INFO_t eval = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	uint32_t legalMoves = 0;
	PIECE_t pieceBackup;
	PIECE_e piece;
	
#if ENABLE_TRANSPOSITION_TABLE
	int hashFlag = ALPHA_HASH_RESULT;
	if (val = probeHash(depth, alpha, beta) != UNKNOWN_HASH_RESULT)
	{
		cacheHits++;
		return val;
	}
	else
	{
		cacheMisses++;	
	}
	
#endif // ENABLE_TRANSPOSITION_TABLE
	
	// calculate all of the moves
	MOVE_t moves[MOVE_LIST_SIZE];
	legalMoves += AddAllLegalMovesToMoveList(moves, &eval, whiteToPlay);

	if (!legalMoves && depth == currentDepth)
	{
		printf("pass");
		exit(0);
	}
	
	if (depth == 0)
	{
		numberEvaluations++;
		AddAllLegalMovesToMoveList(&moves[legalMoves-1], &eval, !whiteToPlay);
		val = evaluateBoard(&eval);
#if ENABLE_TRANSPOSITION_TABLE
		recordHash(depth, val, EXACT_HASH_RESULT);
#endif //ENABLE_TRANSPOSITION_TABLE
		if (!whiteToPlay)
			val = val * -1;
		return val;
	}

	numberNodes++;
	// for each move
	for (i = 0; i < legalMoves; i++)
	{
		// backup piece so we can undo the move later
		piece = (moves[i].move & PIECE_MASK) >> PIECE_SHIFT;
		pieceBackup.piece = pieces[piece].piece;
		
		// play the  move
		playMove(moves, i);
		
		// Check to see if this position is a win
		val = isWin();
		
		if (!whiteToPlay)
		{
			val = val*-1;
		}

		if (val > DRAW_SCORE)
		{
			val = val + depth;
		}
		else if (val < 0)
		{
			val = val - depth;
		}
		else if (val == DRAW_SCORE)
		{
			val = 0;
		}
		else 
		{
			// not a win or draw; search deeper
			//val = alphaBeta(depth - 1, alpha, beta, !whiteToPlay);
			val = -alphaBeta(depth - 1, -beta, -alpha, !whiteToPlay);	
		}
		
		// undo move
		undoMove(pieceBackup, piece);
		
		// alpha beta pruning:

		if (val >= beta)
		{
#if ENABLE_TRANSPOSITION_TABLE
			recordHash(depth, beta, BETA_HASH_RESULT);
#endif //ENABLE_TRANSPOSITION_TABLE			
			return beta;
		}
		if (val > alpha)
		{
			alpha = val;
			if (depth == currentDepth)
			{
				currMove.move = moves[i].move;				
				currentScore = val;
			}
#if ENABLE_TRANSPOSITION_TABLE
			hashFlag = EXACT_HASH_RESULT;
#endif //ENABLE_TRANSPOSITION_TABLE			
		}
	}
	

#if ENABLE_TRANSPOSITION_TABLE
		recordHash(depth, alpha, hashFlag);
#endif //ENABLE_TRANSPOSITION_TABLE		
	return alpha;
}


void initSearch(void)
{
	numberEvaluations = 0;
	numberNodes = 0;
	currentDepth = 0;
	currMove.move = 0;
	currentScore = 0;
}

/*
 initially alpha = -INFINITY, beta=INFINITY
 
 search(position,side,depth,alpha,beta) {
 
 best_score = -INFINITY
 
 for each move {
 
 do_move(position, move)
 
 if ( depth is 0 )   move_score = static_score(position, side)
 else   move_score = - search(position, opponent side, depth-1, -beta, -alpha)
 
 undo_move(position,move)
 
 if ( move_score > best_score )   best_score = move_score
 if ( best_score > alpha )   alpha = best_score
 if ( alpha >= beta )   return alpha
 }
 
 return best_score
 
 }
 */



/*
alpha-beta pruning algorithm

 def minimax(self, depth, alpha, beta):
 """
 Basic Alpha-Beta min-maxing
 """
 val = self.checkWinScore()
 if not val == None:
 return val
 if depth <= 0:
 val = self.evaluate()
 return val
 
 validMoves = self.game.getValidMoves()
 for move in validMoves:
 self.game.makeMove(move)
 
 val = -self.minimax(depth - 1, -alpha, -beta)
 
 self.game.unmakeMove(move)
 
 if val >= beta: # our opponent won't let us get to this move, it's too good
 return beta
 
 if val > alpha: # yea! a better move that we can get to
 alpha = val
 if depth == self.horizonDepth:
 self.bestMove = move
 
 return alpha
 
 
 
 
*/