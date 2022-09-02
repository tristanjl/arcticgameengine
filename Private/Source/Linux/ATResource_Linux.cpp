#include "ATString.h"

void ATResource_GetPath(char* outputPath, int length)
{
    ATString_Copy(outputPath, length, "Media/");
}
