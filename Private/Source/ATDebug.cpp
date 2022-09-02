#include "AT.h"
#include "ATTime.h"
#include "ATString.h"

#include "stdio.h"
#include "stdarg.h"
#include "stdlib.h"

AT_API void ATDebug_LogMessagePlatformSpecific(const char* logMessage, ...);
AT_API bool ATDebug_DisplayAssertPlatformSpecific(const char* assertMessage, ...);

AT_API void ATDebug_LogMessage(ATDebug_LogLevel logLevel, const char* const filename, int lineNumber, const char* outputString, ...)
{
	char logMessageFormat[1024];
	const char* logLevelString;
	switch (logLevel)
	{
	case ATDLL_Information:
	default:
		logLevelString = "Information";
		break;
	case ATDLL_Warning:	logLevelString = "Warning"; break;
	case ATDLL_Error:	logLevelString = "Error"; break;
	case ATDLL_Assert:	logLevelString = "Assert"; break;
	}
	int year, month, day, hour, min, second;
	ATTime_GetCurrent(&year, &month, &day, &hour, &min, &second);
	sprintf(logMessageFormat, "%04i:%02i:%02i %02i:%02i:%02i [%s]: %s (%s, %i)\n", year, month, day, hour, min, second, logLevelString, outputString, filename, lineNumber);
	va_list args;
	va_start(args, outputString);
	char logMessage[1024];
	vsprintf(logMessage, logMessageFormat, args);
	va_end(args);

	ATDebug_LogMessagePlatformSpecific(logMessage);
}

AT_API bool ATDebug_DisplayAssert(const char* const filename, int lineNumber, const char* outputString, ...)
{
	va_list args;
	va_start(args, outputString);
	char assertMessage[1024];
	vsprintf(assertMessage, outputString, args);
	va_end(args);
	ATDebug_LogMessage(ATDLL_Assert, filename, lineNumber, assertMessage);

	return ATDebug_DisplayAssertPlatformSpecific(assertMessage);
}
