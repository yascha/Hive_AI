#include <stdio.h>
#include "debug.h"
#include "types.h"
#include "board.h"
#include "pieces.h"
#include "isWin.h"
#include "time.h"
#include "game.h"

void parseArgs(int argCount);
void cleanup(void);


int main (int argc, const char * argv[]) 
{
	parseArgs(argc);
	
	initState();
	
	//DEBUG("%s\n%s\n", argv[1], argv[2]);
	
	parseTimeString(argv[1]);
	parseMoveStringAndSetupBoardState(argv[2]);
	//printBoard(4, 4);
	calculateBestMove(moveNumber%2 == 1 ? true : false);
	printOutBestMove();

	cleanup();
	
    return 0;
}

void parseArgs(int argCount)
{
	if (argCount != 3)
    {
        printf( "invalid arguments. expected: shiva --times=\"1234,1,2\" --moves=\"1. wB1, 2.bG1 -wB1, 3.etc\"");
		exit(0);
    }
	
}


void cleanup(void)
{
//	if (pieceNamePtr != NULL)
//		free(pieceNamePtr);
//	if (pieceQualifierPtr != NULL)
//		free(pieceQualifierPtr);
}
