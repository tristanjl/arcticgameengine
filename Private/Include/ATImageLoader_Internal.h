#ifndef AT_IMAGE_LOADER_INTERNAL_H
#define AT_IMAGE_LOADER_INTERNAL_H

enum ATImageFormat
{
	ATIF_Bitmap,
	ATIF_Unsupported
};

AT_API uint8* ATImageLoader_Load(const char* filename, int* width, int* height, int* channels);

AT_API uint8* ATImageLoader_LoadFromData(const uint8* const fileData, size_t* dataSize, int* width, int* height, int* channels);

void ATImageLoader_Unload(void* data);

#endif // AT_IMAGE_LOADER_INTERNAL_H
