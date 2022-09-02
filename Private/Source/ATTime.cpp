#include "AT.h"
#include <ctime>

const int64 FRAME_RATE_MICROSECONDS = 16666;
int64 frameTime = 0;

AT_API int64 ATTime_GetFrameRateInMicroseconds()
{
	return FRAME_RATE_MICROSECONDS;
}

AT_API int64 ATTime_GetNextFrameTimeInMicroseconds()
{
	return frameTime;
}

AT_API void ATTime_IncrementFrameTime()
{
	frameTime += ATTime_GetFrameRateInMicroseconds();
}

AT_API void ATTime_GetCurrent(int* year, int* month, int* day, int* hour, int* min, int* second)
{
	time_t t = time(0);   // get time now
    tm* now = localtime(&t);
    *year = now->tm_year + 1900;
    *month = now->tm_mon + 1;
    *day = now->tm_mday;
    *hour = now->tm_hour;
    *min = now->tm_min;
    *second = now->tm_sec;
}
