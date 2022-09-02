#include "AT.h"
#include "string.h"

// djb2 string hash algorithm - http://www.cse.yorku.ca/~oz/hash.html
AT_API size_t ATString_Hash(const char* string, int size)
{
	unsigned int hash = 5381;

	while (*string)
	{
		hash = ((hash << 5) + hash) + *string;
		++string;
	}

	return hash % size;
}

AT_API size_t ATString_Length(const char* string)
{
	size_t count = 0;
	while (*string)
	{
		++count;
		++string;
	}
	return count;
}

AT_API int ATString_Compare(const char* string1, const char* string2)
{
	while (*string1)
	{
		if (*string1 != *string2)
			return *string1 - *string2;

		++string1;
		++string2;
	}
	return *string1 - *string2;
}

AT_API int ATString_CompareCharacters(const char* string1, const char* string2, size_t count)
{
	for (size_t i = 0; i < count - 1 && *string1; ++i)
	{
		if (*string1 != *string2)
			return *string1 - *string2;

		++string1;
		++string2;
	}
	return *string1 - *string2;
}

AT_API void ATString_Copy(char* dest, size_t destinationSize, const char* source)
{
	ATUNSED(destinationSize);
	strcpy(dest, source);
}

AT_API void ATString_CopyCharacters(char* dest, size_t destinationSize, const char* source, size_t count)
{
	ATUNSED(destinationSize);
	strncpy(dest, source, count);
}

AT_API void ATString_Concatenate(char* dest, size_t destinationSize, const char* source)
{
	ATUNSED(destinationSize);
	strcat(dest, source);
}

AT_API const char* ATString_FindChar(const char* string, char c)
{
	while (*string)
	{
		if (*string == c)
			return string;

		++string;
	}
	return 0;
}

AT_API const char* ATString_FindCharReverse(const char* string, char c)
{
	const char* end = string + (ATString_Length(string) - 1);
	while (end != string)
	{
		if (*end == c)
			return end;

		--end;
	}
	if (*end == c)
		return end;
	return 0;
}

AT_API const char* ATString_FindSubStr(const char* string, const char* searchString)
{
	if (!*searchString)
	{
		return 0; // Don't attempt to match an empty string
	}

	while (*string)
	{
		const char* s = string;
		const char* ss = searchString;
		while (*s == *ss)
		{
			++s;
			++ss;
			if (!*ss)
			{
				return string;
			}
		}

		++string;
	}
	return 0;
}
