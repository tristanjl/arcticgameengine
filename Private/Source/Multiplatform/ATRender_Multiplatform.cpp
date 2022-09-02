#include "AT.h"
#include "ATRender.h"
#include "ATRender_Internal.h"
#include "ATVector4.h"

static ATRect s_ViewportRect;

AT_API void ATRender_InitPlatformSpecific()
{
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glAlphaFunc(GL_GREATER, 0.1f);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_DEPTH_TEST);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

AT_API void ATRender_DeinitPlatformSpecific()
{
}

AT_API void ATRender_SetOrthoView(float x, float y, float width, float height, float nearPlane, float farPlane)
{
	s_ViewportRect.x = x;
	s_ViewportRect.y = y;
	s_ViewportRect.width = width;
	s_ViewportRect.height = height;

    glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
#if defined OPENGLES_1_1
	glOrthof(x, x + width, y, y + height, farPlane, nearPlane);
#else
	glOrtho(x, x + width, y, y + height, farPlane, nearPlane);
#endif // IPHONE
	glMatrixMode(GL_MODELVIEW);
}

AT_API void ATRender_Clear(ATClearFlags flags, const ATVector4& clearColor, float depth, int stencil)
{
	glClearColor(clearColor.v.x, clearColor.v.y, clearColor.v.z, clearColor.v.w);

#ifndef OPENGLES_1_1
	glClearDepth(depth);
#else
	glClearDepthf(depth);
#endif // OPENGLES_1_1
	glClearStencil(stencil);
	uint32 glFlags = 0;
	glFlags |= flags & ATCF_Color ? GL_COLOR_BUFFER_BIT : 0;
	glFlags |= flags & ATCF_Depth ? GL_DEPTH_BUFFER_BIT : 0;
	glFlags |= flags & ATCF_Stencil ? GL_STENCIL_BUFFER_BIT : 0;
	glClear(glFlags);

	glDisable(GL_TEXTURE_2D);
}

AT_API const ATRect* ATRender_GetViewportRect()
{
	return &s_ViewportRect;
}
