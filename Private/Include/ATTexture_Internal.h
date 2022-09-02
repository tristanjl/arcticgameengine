#ifndef AT_TEXTURE_INTERNAL_H
#define AT_TEXTURE_INTERNAL_H

struct ATTexture
{
	uint32 textureData;
	int width;
	int height;
	char* textureName;
	int referenceCount;
};

AT_API void ATTexture_CreateFromMemory(ATTexture** texture, const uint8* const data, size_t dataSize, const char* referenceName);

#endif //AT_TEXTURE_INTERNAL_H
