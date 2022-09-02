#include "AT.h"
#include "ATFile.h"
#include "Android/ATSystem_Android_Internal.h"

#include <stdio.h>

// Use empty struct as placeholder for type and simply treat the ATFile as an AAsset
struct ATFile
{
};

AT_API void ATFile_Open(ATFile** file, const char* filename)
{
	android_app* app = ATSystem_Android_GetApp();
	*file = (ATFile*)AAssetManager_open(app->activity->assetManager, filename, AASSET_MODE_BUFFER);
}

AT_API uint64 ATFile_GetSize(ATFile* file)
{
	return (uint64)AAsset_getLength((AAsset*)file);
}

AT_API void ATFile_Close(ATFile* file)
{
	AAsset_close((AAsset*)file);
}

AT_API size_t ATFile_Read(ATFile* file, void* buffer, size_t size)
{
	return AAsset_read((AAsset*)file, buffer, size);
}

AT_API void ATFile_Seek(ATFile* file, size_t offsetFromStart, ATFileSeekMode mode)
{
	AAsset_seek((AAsset*)file, offsetFromStart, mode);
}

AT_API int ATFile_CurrentReadPosition(ATFile* file)
{
	return (int)ATFile_GetSize(file) - AAsset_getRemainingLength((AAsset*)file);
}
