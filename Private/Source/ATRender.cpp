#include "AT.h"

AT_API void ATRender_Init()
{
	void ATRender_InitPlatformSpecific(); ATRender_InitPlatformSpecific();

	void ATTexture_Init(); ATTexture_Init();

	void ATSprite_InitSystem(); ATSprite_InitSystem();

	void ATPrimitive_Init(); ATPrimitive_Init();
}

AT_API void ATRender_Deinit()
{
	void ATPrimitive_Deinit(); ATPrimitive_Deinit();

	void ATSprite_DeinitSystem(); ATSprite_DeinitSystem();

	void ATTexture_Deinit(); ATTexture_Deinit();

	void ATRender_DeinitPlatformSpecific(); ATRender_DeinitPlatformSpecific();
}
