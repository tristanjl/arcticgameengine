#include "AT.h"
#include "ATMath.h"
#include "ATArray.h"
#include "ATMatrix.h"
#include "ATSprite.h"
#include "ATVector4.h"
#include "ATTexture.h"
#include "ATTexture_Internal.h"
#include "ATSprite_Internal.h"
#include "ATRenderElement_Internal.h"

int g_SpriteCount;

const int MAX_SPRITE_VERTEX_COUNT = 1024;

ATArray<ATSpriteSet*>* g_SpriteDrawCommands;

AT_API void ATSprite_InitSystem()
{
	g_SpriteDrawCommands = ATNew(ATArray<ATSpriteSet*>);
}

AT_API void ATSprite_DeinitSystem()
{
	ATASSERT(g_SpriteCount == 0, "Resource Leak: Sprite that was created was not destroyed");

	ATDelete(ATArray<ATSpriteSet*>, g_SpriteDrawCommands);
}

AT_API void ATSprite_Create(ATSprite** sprite)
{
	ATSprite* newSprite = (ATSprite*)ATAlloc(sizeof(ATSprite));
	ATMemSet(newSprite, 0, sizeof(ATSprite));
	++g_SpriteCount;
	*sprite = newSprite;
}

static void ATSpriteSet_SetFrameRange(ATSpriteSet* spriteSet, uint32 rangeStart, uint32 rangeEnd,
									  float pixelXFrameSize, float pixelYFrameSize,
									  float pixelXOffset, float pixelYOffset,
									  uint32 frameXRowFrameCount)
{
	float textureWidth = (float)spriteSet->texture->width;
	float textureHeight = (float)spriteSet->texture->height;

	pixelXFrameSize = pixelXFrameSize == 0.0f ? textureWidth : pixelXFrameSize;
	pixelYFrameSize = pixelYFrameSize == 0.0f ? textureHeight : pixelYFrameSize;

	if (frameXRowFrameCount == 0)
	{
		frameXRowFrameCount = (uint32)(textureWidth / pixelXFrameSize);
	}

	float startX = pixelXOffset / textureWidth;
	float startY = pixelYOffset / textureHeight;

	float frameWidth = pixelXFrameSize / textureWidth;
	float frameHeight = pixelYFrameSize / textureWidth;

	for (uint32 i = rangeStart; i < rangeEnd; ++i)
	{
		float xIndex = (float)(i % frameXRowFrameCount);
		float yIndex = (float)(i / frameXRowFrameCount);
		spriteSet->elementBounds[i].width = frameWidth;
		spriteSet->elementBounds[i].height = frameHeight;
		spriteSet->elementBounds[i].x = startX + xIndex * frameWidth;
		spriteSet->elementBounds[i].y = startY + yIndex * frameHeight;
	}
}

static void ATSpriteSet_Add(ATSpriteSet* spriteSet, uint32 frameCount,
							float pixelXFrameSize, float pixelYFrameSize,
							float pixelXOffset, float pixelYOffset,
							uint32 frameXRowFrameCount)
{
	ATASSERT(frameCount > 0, "Sprite Texture must contain at least one frame");

	uint32 oldElementCount = spriteSet->elementCount;
	uint32 newElementCount = frameCount + oldElementCount;
	ATRect* newElementBounds = (ATRect*)ATAlloc(sizeof(ATRect) * newElementCount);
	ATMemCopy(newElementBounds, spriteSet->elementBounds, sizeof(ATRect) * oldElementCount);
	ATFree(spriteSet->elementBounds);

	spriteSet->elementBounds = newElementBounds;
	spriteSet->elementCount = newElementCount;

	ATSpriteSet_SetFrameRange(spriteSet, oldElementCount, newElementCount, pixelXFrameSize, pixelYFrameSize, pixelXOffset, pixelYOffset, frameXRowFrameCount);
}

static void ATSpriteSet_Init(ATSpriteSet* spriteSet, ATTexture* texture, uint32 frameCount,
							 float pixelXFrameSize, float pixelYFrameSize,
							 float pixelXOffset, float pixelYOffset,
							 uint32 frameXRowFrameCount)
{
	ATASSERT(frameCount > 0, "Sprite Texture must contain at least one frame");

	ATMemSet(spriteSet, 0, sizeof(ATSpriteSet));

	spriteSet->texture = texture;
	++texture->referenceCount; // ATSprite frees textures when destroyed. Add one to the reference count to account for this
	spriteSet->elementBounds = (ATRect*)ATAlloc(sizeof(ATRect) * frameCount);
	spriteSet->elementCount = frameCount;

	ATSpriteSet_SetFrameRange(spriteSet, 0, frameCount, pixelXFrameSize, pixelYFrameSize, pixelXOffset, pixelYOffset, frameXRowFrameCount);
}

static void ATSpriteSet_Init(ATSpriteSet* spriteSet, const char* textureFilename, uint32 frameCount,
					  float pixelXFrameSize, float pixelYFrameSize,
					  float pixelXOffset, float pixelYOffset,
					  uint32 frameXRowFrameCount)
{
	ATASSERT(frameCount > 0, "Sprite Texture must contain at least one frame");

	ATMemSet(spriteSet, 0, sizeof(ATSpriteSet));

	ATTexture_Create(&spriteSet->texture, textureFilename);
	spriteSet->elementBounds = (ATRect*)ATAlloc(sizeof(ATRect) * frameCount);
	spriteSet->elementCount = frameCount;

	ATSpriteSet_SetFrameRange(spriteSet, 0, frameCount, pixelXFrameSize, pixelYFrameSize, pixelXOffset, pixelYOffset, frameXRowFrameCount);
}

static void ATSpriteSet_Deinit(ATSpriteSet* spriteSet)
{
	if (spriteSet->verticies)
	{
		ATFree(spriteSet->verticies);
	}
	ATTexture_Destroy(spriteSet->texture);
	ATFree(spriteSet->elementBounds);
}

static void ATSprite_AddToSpriteSetArray(ATSprite* sprite)
{
	// Copy from previous - not using ATArray as sprites are intended to be loaded via future reflection system
	size_t newSetCount = sprite->setCount + 1;
	ATSpriteSet* spriteSetArray = (ATSpriteSet*)ATAlloc(sizeof(ATSpriteSet) * newSetCount);
	if (sprite->spriteSets)
	{
		ATMemCopy(spriteSetArray, sprite->spriteSets, sizeof(ATSpriteSet) * sprite->setCount);
		ATFree(sprite->spriteSets);
	}
	sprite->spriteSets = spriteSetArray;
	sprite->setCount = newSetCount;
}

AT_API void ATSprite_Destroy(ATSprite* sprite)
{
	if (sprite->spriteSets)
	{
		for (uint32 i = 0; i < sprite->setCount; ++i)
		{
			ATSpriteSet_Deinit(&sprite->spriteSets[i]);
		}
		ATFree(sprite->spriteSets);
	}
	ATFree(sprite);
	--g_SpriteCount;
}

AT_API void ATSprite_AddSpriteSet(ATSprite* sprite, const char* textureFilename, uint32 frameCount,
						   float pixelXFrameSize, float pixelYFrameSize,
						   float pixelXOffset, float pixelYOffset,
						   uint32 frameXRowWidth)
{
	// Check if the previous sprite set is using the same texture
	if (sprite->setCount > 0 && sprite->spriteSets[sprite->setCount - 1].texture->textureName == textureFilename)
	{
		ATSpriteSet_Add(&sprite->spriteSets[sprite->setCount - 1], frameCount, pixelXFrameSize, pixelYFrameSize, pixelXOffset, pixelYOffset, frameXRowWidth);
	}

	ATSprite_AddToSpriteSetArray(sprite);

	// Create our new sprite set so that we can set it up
	ATSpriteSet_Init(&sprite->spriteSets[sprite->setCount - 1], textureFilename, frameCount, pixelXFrameSize, pixelYFrameSize, pixelXOffset, pixelYOffset, frameXRowWidth);
}

AT_API void ATSprite_AddSpriteSetTexture(ATSprite* sprite, ATTexture* texture, uint32 frameCount,
										 float pixelXFrameSize, float pixelYFrameSize,
										 float pixelXOffset, float pixelYOffset,
										 uint32 frameXRowWidth)
{
	// Check if the previous sprite set is using the same texture
	if (sprite->setCount > 0 && sprite->spriteSets[sprite->setCount - 1].texture == texture)
	{
		ATSpriteSet_Add(&sprite->spriteSets[sprite->setCount - 1], frameCount, pixelXFrameSize, pixelYFrameSize, pixelXOffset, pixelYOffset, frameXRowWidth);
	}

	ATSprite_AddToSpriteSetArray(sprite);

	// Create our new sprite set so that we can set it up
	ATSpriteSet_Init(&sprite->spriteSets[sprite->setCount - 1], texture, frameCount, pixelXFrameSize, pixelYFrameSize, pixelXOffset, pixelYOffset, frameXRowWidth);
}

AT_API void ATSpriteSet_Flush(ATSpriteSet* spriteSet)
{
	ATRenderElement element;
#ifdef WINDOWS_STORE
#else
	element.verticies = spriteSet->verticies;
#endif
	element.format = ATVF_PUC;
	element.texture = spriteSet->texture;
	element.transformation = &ATMatrix::Identity;
	element.vertexCount = spriteSet->vertexCount;
	ATRenderElement_Render(&element);

	spriteSet->vertexCount = 0;
}

AT_API void ATSprite_Render(ATSprite* sprite, const ATVector4& position, size_t frameIndex, ATSpritePivotFlags pivotFlags, float scaleX, float scaleY, float rotation, const ATVector4& color)
{
    ATSpriteSet* activeSpriteSet = nullptr;
    ATRect* activeFrameRect = nullptr;
	size_t setFrame = frameIndex;
	for (uint32 i = 0; i < sprite->setCount; ++i)
	{
		size_t setElementCount = sprite->spriteSets[i].elementCount;
		if (setFrame < setElementCount)
		{
			activeSpriteSet = &sprite->spriteSets[i];
			activeFrameRect = &sprite->spriteSets[i].elementBounds[setFrame];
			break;
		}
		setFrame -= setElementCount;
	}

	ATASSERT(activeSpriteSet && activeFrameRect, "Invalid frame specified");

#ifdef WINDOWS_STORE
#else
	if (activeSpriteSet->vertexCount >= MAX_SPRITE_VERTEX_COUNT)
	{
		ATSpriteSet_Flush(activeSpriteSet);
	}
	size_t vertexIndex = activeSpriteSet->vertexCount;
	if ((int)activeSpriteSet->vertexCount > (int)activeSpriteSet->vertexBufferSize - 4)
	{
		ATASSERT(activeSpriteSet->vertexCount == activeSpriteSet->vertexBufferSize, "Invalid vertex buffer size in ATSpriteSet");

		size_t newVertexBufferSize;
		if (activeSpriteSet->vertexBufferSize == 0)
		{
			activeSpriteSet->vertexBufferSize = 4;
			activeSpriteSet->verticies = (ATVertex_PUC*)ATAlloc(activeSpriteSet->vertexBufferSize * sizeof(ATVertex_PUC));
		}
		else
		{
			newVertexBufferSize = activeSpriteSet->vertexBufferSize + 6;
			vertexIndex = activeSpriteSet->vertexCount + 2;
			ATVertex_PUC* newVerticies = (ATVertex_PUC*)ATAlloc(newVertexBufferSize * sizeof(ATVertex_PUC));
			ATMemCopy(newVerticies, activeSpriteSet->verticies, activeSpriteSet->vertexCount * sizeof(ATVertex_PUC));
			ATFree(activeSpriteSet->verticies);
			activeSpriteSet->verticies = newVerticies;
			activeSpriteSet->vertexBufferSize = (uint32)newVertexBufferSize;
		}
	}
	else if (vertexIndex != 0)
	{
		vertexIndex = activeSpriteSet->vertexCount + 2;
	}

	float vertexOffsets[8];
	float frameWidth = activeFrameRect->width * (float)activeSpriteSet->texture->width;
	float frameHeight = activeFrameRect->height * (float)activeSpriteSet->texture->height;
	switch (pivotFlags)
	{
	case ATSPF_Center:
		{
			float halfWidth = frameWidth / 2.0f;
			float halfHeight = frameHeight / 2.0f;
			vertexOffsets[0] = -halfWidth; vertexOffsets[2] = halfWidth; vertexOffsets[4] = -halfWidth; vertexOffsets[6] = halfWidth;
			vertexOffsets[1] = halfHeight; vertexOffsets[3] = halfHeight; vertexOffsets[5] = -halfHeight; vertexOffsets[7] = -halfHeight;
		}
		break;
	case ATSPF_TopLeft:
		vertexOffsets[0] = 0.0f; vertexOffsets[2] = frameWidth; vertexOffsets[4] = 0.0f; vertexOffsets[6] = frameWidth;
		vertexOffsets[1] = 0.0f; vertexOffsets[3] = 0.0f; vertexOffsets[5] = -frameHeight; vertexOffsets[7] = -frameHeight;
		break;
	case ATSPF_TopRight:
		vertexOffsets[0] = -frameWidth; vertexOffsets[2] = 0.0f; vertexOffsets[4] = -frameWidth; vertexOffsets[6] = 0.0f;
		vertexOffsets[1] = 0.0f; vertexOffsets[3] = 0.0f; vertexOffsets[5] = -frameHeight; vertexOffsets[7] = -frameHeight;
		break;
	case ATSPF_BottomLeft:
		vertexOffsets[0] = 0.0f; vertexOffsets[2] = frameWidth; vertexOffsets[4] = 0.0f; vertexOffsets[6] = frameWidth;
		vertexOffsets[1] = frameHeight; vertexOffsets[3] = frameHeight; vertexOffsets[5] = 0.0f; vertexOffsets[7] = 0.0f;
		break;
	case ATSPF_BottomRight:
		vertexOffsets[0] = -frameWidth; vertexOffsets[2] = 0.0f; vertexOffsets[4] = -frameWidth; vertexOffsets[6] = 0.0f;
		vertexOffsets[1] = frameHeight; vertexOffsets[3] = frameHeight; vertexOffsets[5] = 0.0f; vertexOffsets[7] = 0.0f;
		break;
	default:
		ATASSERTRETURN(false, , "Invalid Positioning Flags");
	}
	uint8 colorValue[4];
	colorValue[0] = (uint8)(color.v.x * 255.0f);
	colorValue[1] = (uint8)(color.v.y * 255.0f);
	colorValue[2] = (uint8)(color.v.z * 255.0f);
	colorValue[3] = (uint8)(color.v.w * 255.0f);

	activeSpriteSet->verticies[vertexIndex].uv[0] = activeFrameRect->x;
	activeSpriteSet->verticies[vertexIndex].uv[1] = activeFrameRect->y;
	*((uint32 *)activeSpriteSet->verticies[vertexIndex].color) = *((uint32 *)colorValue);
	activeSpriteSet->verticies[vertexIndex].position[0] = position.v.x + (vertexOffsets[0] * ATMath_Cos(rotation) + vertexOffsets[1] * ATMath_Sin(rotation)) * scaleX;
	activeSpriteSet->verticies[vertexIndex].position[1] = position.v.y - (vertexOffsets[0] * ATMath_Sin(rotation) - vertexOffsets[1] * ATMath_Cos(rotation)) * scaleY;
	activeSpriteSet->verticies[vertexIndex].position[2] = position.v.z;

	activeSpriteSet->verticies[vertexIndex + 1].uv[0] = activeFrameRect->x + activeFrameRect->width;
	activeSpriteSet->verticies[vertexIndex + 1].uv[1] = activeFrameRect->y;
	*((uint32 *)activeSpriteSet->verticies[vertexIndex + 1].color) = *((uint32 *)colorValue);
	activeSpriteSet->verticies[vertexIndex + 1].position[0] = position.v.x + (vertexOffsets[2] * ATMath_Cos(rotation) + vertexOffsets[3] * ATMath_Sin(rotation)) * scaleX;
	activeSpriteSet->verticies[vertexIndex + 1].position[1] = position.v.y - (vertexOffsets[2] * ATMath_Sin(rotation) - vertexOffsets[3] * ATMath_Cos(rotation)) * scaleY;
	activeSpriteSet->verticies[vertexIndex + 1].position[2] = position.v.z;

	activeSpriteSet->verticies[vertexIndex + 2].uv[0] = activeFrameRect->x;
	activeSpriteSet->verticies[vertexIndex + 2].uv[1] = activeFrameRect->y + activeFrameRect->height;
	*((uint32 *)activeSpriteSet->verticies[vertexIndex + 2].color) = *((uint32 *)colorValue);
	activeSpriteSet->verticies[vertexIndex + 2].position[0] = position.v.x + (vertexOffsets[4] * ATMath_Cos(rotation) + vertexOffsets[5] * ATMath_Sin(rotation)) * scaleX;
	activeSpriteSet->verticies[vertexIndex + 2].position[1] = position.v.y - (vertexOffsets[4] * ATMath_Sin(rotation) - vertexOffsets[5] * ATMath_Cos(rotation)) * scaleY;
	activeSpriteSet->verticies[vertexIndex + 2].position[2] = position.v.z;

	activeSpriteSet->verticies[vertexIndex + 3].uv[0] = activeFrameRect->x + activeFrameRect->width;
	activeSpriteSet->verticies[vertexIndex + 3].uv[1] = activeFrameRect->y + activeFrameRect->height;
	*((uint32 *)activeSpriteSet->verticies[vertexIndex + 3].color) = *((uint32 *)colorValue);
	activeSpriteSet->verticies[vertexIndex + 3].position[0] = position.v.x + (vertexOffsets[6] * ATMath_Cos(rotation) + vertexOffsets[7] * ATMath_Sin(rotation)) * scaleX;
	activeSpriteSet->verticies[vertexIndex + 3].position[1] = position.v.y - (vertexOffsets[6] * ATMath_Sin(rotation) - vertexOffsets[7] * ATMath_Cos(rotation)) * scaleY;
	activeSpriteSet->verticies[vertexIndex + 3].position[2] = position.v.z;

	if (!activeSpriteSet->addedThisFrame)
	{
		activeSpriteSet->addedThisFrame = true;
		g_SpriteDrawCommands->Add(activeSpriteSet);
	}

	if (activeSpriteSet->vertexCount == 0)
	{
		activeSpriteSet->vertexCount = 4;
	}
	else
	{
		activeSpriteSet->vertexCount += 6;
		ATMemCopy(&activeSpriteSet->verticies[vertexIndex - 1], &activeSpriteSet->verticies[vertexIndex], sizeof(*activeSpriteSet->verticies));
		ATMemCopy(&activeSpriteSet->verticies[vertexIndex - 2], &activeSpriteSet->verticies[vertexIndex - 3], sizeof(*activeSpriteSet->verticies));
	}
#endif
}

AT_API void ATSprite_Flush()
{
	for (size_t i = 0; i < g_SpriteDrawCommands->Count(); ++i)
	{
		if ((*g_SpriteDrawCommands)[i]->vertexCount)
		{
			ATSpriteSet_Flush((*g_SpriteDrawCommands)[i]);
		}
		(*g_SpriteDrawCommands)[i]->addedThisFrame = false;
	}

	g_SpriteDrawCommands->Clear();
}
