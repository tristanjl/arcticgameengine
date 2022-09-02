#ifndef AT_SPRITE_H
#define AT_SPRITE_H

struct ATSprite;
struct ATTexture;

#include "ATVector4.h"

enum ATSpritePivotFlags
{
	ATSPF_Center,
	ATSPF_TopLeft,
	ATSPF_TopRight,
	ATSPF_BottomLeft,
	ATSPF_BottomRight
};

// Creates an empty sprite object
AT_API void ATSprite_Create(ATSprite** sprite);

AT_API void ATSprite_Destroy(ATSprite* sprite);

// Adds a sequence of sections of a texture to a sprite
// textureFileName - the filename of the texture
// frameCount - the number of frames to be added to the sprite from this texture
// pixelXSize - the width of a frame in the texture in pixels. 0 if the whole image width is used
// pixelYSize - the height of a frame in the texture in pixels. 0 if the whole image height is used
// pixelXOffset - the height of a frame in the texture in pixels. 0 if the whole image height is used
// pixelYOffset - the height of a frame in the texture in pixels. 0 if the whole image height is used

AT_API void ATSprite_AddSpriteSet(ATSprite* sprite,
								  const char* textureFilename,
								  uint32 frameCount = 1,
								  float pixelXSize = 0.0f,
								  float pixelYSize = 0.0f,
								  float pixelXOffset = 0.0f,
								  float pixelYOffset = 0.0f,
								  uint32 frameXRowWidth = 0);

AT_API void ATSprite_AddSpriteSetTexture(ATSprite* sprite,
										 ATTexture* texture,
										 uint32 frameCount = 1,
										 float pixelXSize = 0.0f,
										 float pixelYSize = 0.0f,
										 float pixelXOffset = 0.0f,
										 float pixelYOffset = 0.0f,
										 uint32 frameXRowWidth = 0);

AT_API void ATSprite_Render(ATSprite* sprite,
							const ATVector4& position,
							size_t frameIndex = 0,
							ATSpritePivotFlags pivotFlags = ATSPF_Center,
							float scaleX = 1.0f,
							float scaleY = 1.0f,
							float rotation = 0.0f,
							const ATVector4& color = ATVector4::White);

#endif // AT_SPRITE_H
