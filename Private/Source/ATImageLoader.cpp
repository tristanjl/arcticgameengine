#include "AT.h"
#include "ATMath.h"
#include "ATString.h"
#include "ATImageLoader_Internal.h"

#include "SOIL/SOIL.h"
#include "SOIL/image_helper.h"

#include "stdlib.h"

uint8* ATImageLoader_UpscaleToPower2(const uint8* const data, int* width, int* height, int channels)
{
	size_t newWidth = ATMath_GetNextPowerOf2((size_t)*width);
	size_t newHeight = ATMath_GetNextPowerOf2((size_t)*height);
	uint8* scaledData = (uint8*)ATAlloc(newWidth * newHeight * 12);
	up_scale_image(data, *width, *height, channels, scaledData, (int)newWidth, (int)newHeight);
	*width = (int)newWidth;
	*height = (int)newHeight;
	return scaledData;
}

uint8* ATImageLoader_Load(const char* filename, int* width, int* height, int* channels)
{
	uint8* data = SOIL_load_image(filename, width, height, channels, SOIL_LOAD_RGBA);
	ATASSERT(data, "Unable to load image file \"%s\": %s", filename, SOIL_last_result());

	if (!ATISPOWEROF2(*width) || !ATISPOWEROF2(*height))
	{
		ATASSERT(false, "Texture Not a power of 2, upscaling");
		uint8* scaledData = ATImageLoader_UpscaleToPower2(data, width, height, *channels);
		ATFree(data);
		return scaledData;
	}
	return data;
}

uint8* ATImageLoader_LoadFromData(const uint8* const fileData, size_t* dataSize, int* width, int* height, int* channels)
{
	uint8* data = SOIL_load_image_from_memory(fileData, (int)*dataSize, width, height, channels, SOIL_LOAD_RGBA);
	if (!ATISPOWEROF2(*width) || !ATISPOWEROF2(*height))
	{
		ATASSERT(false, "Texture Not a power of 2, upscaling");
		uint8* scaledData = ATImageLoader_UpscaleToPower2(data, width, height, *channels);
		ATFree(data);
		*dataSize = (size_t)((*width) * (*height) * 12);
		return scaledData;
	}
	*dataSize = (size_t)((*width) * (*height) * 12);
	return data;
}

void ATImageLoader_Unload(void* data)
{
	free(data);
}
