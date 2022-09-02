#include "AT.h"
#include "ATMath.h"
#include "ATResource.h"
#include "ATHashTable.h"
#include "ATRender_Internal.h"
#include "ATTexture_Internal.h"
#include "ATImageLoader_Internal.h"

typedef ATHashTable<ATTexture, 32> TextureHashTable;
TextureHashTable* g_TextureTable;

AT_API void ATTexture_Init()
{
	g_TextureTable = ATNew(TextureHashTable);
}

AT_API void ATTexture_Deinit()
{
	ATASSERT(g_TextureTable->Count() == 0, "Resource Leak: Texture that was created was not destroyed");

	ATDelete(TextureHashTable, g_TextureTable);
}

static void ATTexture_AddTexture(ATTexture* storedTexture, uint8* data, const char* const name)
{
	GLuint textureReference = 0;
	glGenTextures(1, &textureReference);
	glBindTexture(GL_TEXTURE_2D, textureReference);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, storedTexture->width, storedTexture->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	size_t filenameLength = ATString_Length(name);
	++filenameLength;
	storedTexture->textureName = (char*)ATAlloc(filenameLength);
	ATString_Copy(storedTexture->textureName, filenameLength, name);
	storedTexture->textureData = textureReference;
	storedTexture->referenceCount = 0;
	g_TextureTable->Add(name, storedTexture);
}

AT_API void ATTexture_CreateFromMemory(ATTexture** texture, const uint8* const data, size_t dataSize, const char* referenceName)
{
	ATTexture* storedTexture = g_TextureTable->Get(referenceName);
	if (!storedTexture)
	{
		storedTexture = (ATTexture*)ATAlloc(sizeof(ATTexture));

		int channels;

		uint8 *textureData = ATImageLoader_LoadFromData(data, &dataSize, &storedTexture->width, &storedTexture->height, &channels);
		ATTexture_AddTexture(storedTexture, textureData, referenceName);
		ATImageLoader_Unload(textureData);
	}
	++storedTexture->referenceCount;
	*texture = storedTexture;
}

AT_API void ATTexture_Create(ATTexture** texture, const char* filename)
{
	ATTexture* storedTexture = g_TextureTable->Get(filename);
	if (!storedTexture)
	{
		char filePath[512];
		ATResource_GetPath(filePath, ATARRAYSIZE(filePath));
		ATString_Concatenate(filePath, ATARRAYSIZE(filePath), filename);

		storedTexture = (ATTexture*)ATAlloc(sizeof(ATTexture));

		int channels;

		uint8 *textureData = ATImageLoader_Load(filePath, &storedTexture->width, &storedTexture->height, &channels);
		ATTexture_AddTexture(storedTexture, textureData, filename);
		ATImageLoader_Unload(textureData);
	}
	++storedTexture->referenceCount;
	*texture = storedTexture;
}

AT_API void ATTexture_Destroy(ATTexture* texture)
{
	--texture->referenceCount;
	if (texture->referenceCount <= 0)
	{
		g_TextureTable->Remove(texture->textureName);
		ATFree(texture->textureName);
		ATFree(texture);
	}
}

struct ATSpriteTextureSection
{
	ATTexture texture;
	ATRect uvBounds;
};

struct ATSprite
{
};
