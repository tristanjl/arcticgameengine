#include "AT.h"
#include "ATFile.h"

#include "stdio.h"

// Use empty struct as placeholder for type and simply treat the ATFile as a FILE*
struct ATFile
{
};

AT_API void ATFile_Open(ATFile** file, const char* filename)
{
    file = (ATFile**)fopen(filename, "rb");
}

AT_API uint64 ATFile_GetSize(ATFile* file)
{
	int current = ftell((FILE*)file);
	ATFile_Seek(file, 0, ATFSM_End);
	int size = ftell((FILE*)file);
	ATFile_Seek(file, (size_t)current, ATFSM_Start);
	return (uint64)size;
}

AT_API void ATFile_Close(ATFile* file)
{
	fclose((FILE*)file);
}

AT_API size_t ATFile_Read(ATFile* file, void* buffer, size_t size)
{
	return fread(buffer, 1, size,(FILE*)file);
}

AT_API void ATFile_Seek(ATFile* file, size_t offsetFromStart, ATFileSeekMode mode)
{
	fseek((FILE*)file, (int)offsetFromStart, mode);
}

AT_API int ATFile_CurrentReadPosition(ATFile* file)
{
	return ftell((FILE*)file);
}
