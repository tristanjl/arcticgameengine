#ifndef AT_STRING_H
#define AT_STRING_H

#include "ATTargets.h"

#include <cstring>

AT_API size_t ATString_Hash(const char* string, size_t size);

AT_API size_t ATString_Length(const char* string);

AT_API int ATString_Compare(const char* string1, const char* string2);

AT_API int ATString_CompareCharacters(const char* string1, const char* string2, size_t count);

AT_API void ATString_Copy(char* dest, size_t destinationSize, const char* source);

AT_API void ATString_CopyCharacters(char* dest, size_t destinationSize, const char* source, size_t count);

AT_API void ATString_Concatenate(char* dest, size_t destinationSize, const char* source);

AT_API const char* ATString_FindChar(const char* string, char c);

AT_API const char* ATString_FindCharReverse(const char* string, char c);

AT_API const char* ATString_FindSubStr(const char* string, const char* searchString);

#endif // AT_STRING
