#ifndef AT_RENDER_H
#define AT_RENDER_H

struct ATVector4;

AT_API void ATRender_SetOrthoView(float x, float y, float width, float height, float nearPlane, float farPlane);

enum ATClearFlags
{
	ATCF_Color = ATBIT(0),
	ATCF_Depth = ATBIT(1),
	ATCF_Stencil = ATBIT(2),

	ATCF_All = ATCF_Color | ATCF_Depth | ATCF_Depth,

	ATCF_MAX
};

AT_API void ATRender_Clear(ATClearFlags flags, const ATVector4 &clearColor, float depth = 1.0f, int stencil = 0);

#endif // AT_RENDER_H
