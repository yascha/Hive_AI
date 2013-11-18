
#include "time.h"
#include "debug.h"

void parseTimeString(char* timeString)
{
	char* totalTime;
	char* whiteUsedTime;
	char* blackUsedTime;
	
	totalTime = strtok(timeString, "=,");
	totalTime = strtok(NULL, ",");
	whiteUsedTime = strtok(NULL, ",");
	blackUsedTime = strtok(NULL, ",");
	
	totalGameTimePerPlayer = (uint32_t) (atol(totalTime) / 2);
	whiteRemainingTime = totalGameTimePerPlayer - (uint32_t)atol(whiteUsedTime);
	blackRemainingTime = totalGameTimePerPlayer - (uint32_t)atol(blackUsedTime);

	//DEBUG("Total time per player(ms):%u\nWhite remaining time(ms):%u\nBlack remaining time(ms):%u\n", totalGameTimePerPlayer, whiteRemainingTime, blackRemainingTime);
}