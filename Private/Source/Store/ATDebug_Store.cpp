#include "AT.h"

AT_API bool ATDebug_DisplayAssertPlatformSpecific(const char* /*outputString*/)
{
	//int result = MessageBox(NULL, outputString, "Assert", MB_ABORTRETRYIGNORE | MB_DEFBUTTON2 | MB_ICONWARNING);

	//switch (result)
	//{
	//case IDABORT:
	//	{
	//		exit(0);
	//	}
	//case IDRETRY:
	//	{
	//		return true;
	//	}
	//case IDIGNORE:
	//	{
	//		return false;
	//	}
	//}
	return false;
}
