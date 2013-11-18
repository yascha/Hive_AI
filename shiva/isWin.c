

#include "isWin.h"
#include "pieces.h"
#include "board.h"

// Return +1 for white win, -1 fo black win, 0 for no win
int isWin(void)
{
	int winScore = 0;
	uint8_t xpos, ypos;
	bool blackwin = false;

	xpos = getXpos(WHITE_QUEEN);
	ypos = getYpos(WHITE_QUEEN);
	if (6 == getNumberOfOccupiedSquaresAroundCoords(xpos, ypos))
	{
		// black win
		winScore = BLACK_WIN_SCORE;
		blackwin = true;
	}
	
	xpos = getXpos(BLACK_QUEEN);
	ypos = getYpos(BLACK_QUEEN);
	if (6 == getNumberOfOccupiedSquaresAroundCoords(xpos, ypos))
	{
		// white win
		winScore = WHITE_WIN_SCORE;
		if (blackwin)
			winScore = DRAW_SCORE;
	}
	
	return winScore;
}

