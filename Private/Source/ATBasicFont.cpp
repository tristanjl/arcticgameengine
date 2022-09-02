#include "AT.h"
#include "ATSprite.h"
#include "ATTexture.h"
#include "ATBasicFont.h"
#include "ATBasicFontData.h"
#include "ATTexture_Internal.h"
#include "ATImageLoader_Internal.h"

static ATSprite* g_ATBasicFont_Sprite;
static ATTexture* g_ATBasicFont_Texture;
static float g_TextPixelWidth = 30.0f;
static float g_TextPixelHeight = 60.0f;
static float g_DefaultFontSize = 72.0f;

AT_API void ATBasicFont_Init()
{
	ATSprite_Create(&g_ATBasicFont_Sprite);
	size_t dataSize = ATARRAYSIZE(ATBasicFontData);
	ATTexture_CreateFromMemory(&g_ATBasicFont_Texture, ATBasicFontData, dataSize,"ATBasicFont.Internal");
	ATSprite_AddSpriteSetTexture(g_ATBasicFont_Sprite, g_ATBasicFont_Texture, 136, 30.0f, 60.0f, 0.0f, 0.0f, 17);
}

AT_API void ATBasicFont_Deinit()
{
	ATSprite_Destroy(g_ATBasicFont_Sprite);
	ATTexture_Destroy(g_ATBasicFont_Texture);
}

AT_API void ATBasicFont_RenderText(const ATVector4& position, char* text, float fontSize, const ATVector4& color)
{
	ATVector4 currentPosition = position;
	while (*text)
	{
		if (*text >= '!' && *text <= '~')
		{
			ATSprite_Render(g_ATBasicFont_Sprite, currentPosition, (size_t)(*text - '!'), ATSPF_TopLeft, fontSize / g_DefaultFontSize, fontSize / g_DefaultFontSize, 0.0f, color);
			currentPosition.v.x += g_TextPixelWidth * fontSize / g_DefaultFontSize;
		}
		else if (*text == '\r' || *text == '\n')
		{
			if (*text == '\r')
			{
				char* checkNext = text;
				++checkNext;
				if (*checkNext == '\n')
				{
					++text;
				}
			}
			currentPosition.v.y -= g_TextPixelHeight * fontSize / g_DefaultFontSize;
			currentPosition.v.x = position.v.x;
		}
		else if (*text == ' ')
		{
			currentPosition.v.x += g_TextPixelWidth * fontSize / g_DefaultFontSize;
		}
		else
		{
			ATASSERT(false, "Unsupported character %c (character code: %u) encountered in text", *text, (int)*text);
		}
		++text;
	}
}
