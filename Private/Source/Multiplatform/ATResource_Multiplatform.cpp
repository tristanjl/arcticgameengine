#include "AT.h"
#include "ATString.h"

void ATResource_GetPath(char* outputPath, size_t length)
{
    ATString_Copy(outputPath, length, "/Media/");
}
