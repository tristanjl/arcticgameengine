#include "AT.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

AT_API int64 ATTime_GetSystemTimeInMicroseconds()
{
	LARGE_INTEGER Frequency;

	if (QueryPerformanceFrequency(&Frequency))
	{
		LARGE_INTEGER currentTime;

		// Start time measurement
		QueryPerformanceCounter(&currentTime);

		// To get duration in microseconds
		return currentTime.QuadPart * 1000000 / Frequency.QuadPart;
	}
	return 0;
}
