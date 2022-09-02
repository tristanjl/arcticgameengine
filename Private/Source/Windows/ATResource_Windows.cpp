#include "AT.h"
#include "ATString.h"

#include <direct.h>

void ATResource_GetPath(char* outputPath, size_t length)
{
	_getcwd(outputPath, (int)length);
	ATString_Concatenate(outputPath, length, "\\Media\\");
}
