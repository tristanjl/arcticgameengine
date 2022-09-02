#include "AT.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

void ATDebug_LogMessagePlatformSpecific(const char* logMessage)
{
	OutputDebugString(logMessage);
}

bool ATDebug_DisplayAssertPlatformSpecific(const char* assertMessage)
{
	int result = MessageBox(NULL, assertMessage, "Assert", MB_ABORTRETRYIGNORE | MB_DEFBUTTON2 | MB_ICONWARNING);

	switch (result)
	{
	case IDABORT:
		{
			exit(0);
		}
	case IDRETRY:
		{
			return true;
		}
	case IDIGNORE:
		{
			return false;
		}
	}
	return false;
}
