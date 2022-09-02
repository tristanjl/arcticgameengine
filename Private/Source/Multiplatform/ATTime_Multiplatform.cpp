#include "AT.h"
#include <ctime>

AT_API int64 ATTime_GetSystemTimeInMicroseconds()
{
	uint64 counter = (uint64)clock();
	return counter * 1000000 / CLOCKS_PER_SEC;
}
