#include "AT.h"

#include <stdio.h>
#include <cstdarg>

AT_API void ATDebug_LogMessagePlatformSpecific(const char* outputString, ...)
{
	va_list args;
	va_start(args, outputString);
	vprintf(outputString, args);
    va_end(args);
}

AT_API bool ATDebug_DisplayAssertPlatformSpecific(const char* outputString, ...)
{
    va_list args;
    va_start(args, outputString);
    vprintf(outputString, args);
    va_end(args);
    return true;
}
