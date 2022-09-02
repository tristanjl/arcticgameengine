#ifndef AT_FILE_H
#define AT_FILE_H

struct ATFile;
enum ATFileSeekMode
{
	ATFSM_Start,
	ATFSM_Current,
	ATFSM_End
};

AT_API void ATFile_Open(ATFile** file, const char* filename);

AT_API uint64 ATFile_GetSize(ATFile* file);

AT_API void ATFile_Close(ATFile* file);

AT_API size_t ATFile_Read(ATFile* file, void* buffer, size_t size);

AT_API void ATFile_Seek(ATFile* file, size_t offsetFromStart, ATFileSeekMode seekMode);

AT_API int ATFile_CurrentReadPosition(ATFile* file);

#endif // AT_FILE_H
