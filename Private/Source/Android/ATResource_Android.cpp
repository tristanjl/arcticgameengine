#include "AT.h"
#include "ATString.h"

void ATResource_GetPath(char* outputPath, size_t size)
{
	ATString_Copy(outputPath, size, "");
}
