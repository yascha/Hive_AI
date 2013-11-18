
#include "clock.h"

void startTimer(void)
{
    searchTime = getTime();	
}


int getSearchTime(void)
{
	return (getTime() - searchTime);
}

int getTime()
{
    
    double timeTaken; 
    clock_t mark;
    mark = clock();
	
	timeTaken = (double) mark;
	timeTaken = (1000 * timeTaken) / CLOCKS_PER_SEC;
    return (int) timeTaken;
}