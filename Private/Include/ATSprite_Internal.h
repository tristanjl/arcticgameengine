#ifndef AT_SPRITE_INTERNAL_H
#define AT_SPRITE_INTERNAL_H

struct ATTexture;
struct ATVertex_PUC;

struct ATSpriteSet
{
	ATVertex_PUC* verticies;
	ATTexture* texture;
	ATRect* elementBounds;
	uint32 elementCount;
	uint32 vertexCount;
	uint32 vertexBufferSize;
	bool addedThisFrame;
};

struct ATSprite
{
	ATSpriteSet* spriteSets;
	size_t setCount;
};

#endif // AT_SPRITE_INTERNAL_H

