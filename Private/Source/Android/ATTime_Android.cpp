#include "AT.h"
#include <sys/timeb.h>

AT_API int64 ATTime_GetSystemTimeInMicroseconds()
{
	timeb buffer;
	ftime(&buffer);
	int64 currentTime = buffer.time * 1000000 + buffer.millitm * 1000;
	return currentTime;
}
