#ifndef AT_RENDER_INTERNAL_H
#define AT_RENDER_INTERNAL_H


#ifdef WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#pragma comment(lib, "opengl32.lib")
#endif

#ifdef WINDOWS_GCC
#define WIN32_LEAN_AND_MEAN
#include "w32api/windows.h"
#include "w32api/GL/gl.h"
#include "w32api/GL/glu.h"
#elif defined (IPHONE)
#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>
#define glOrtho glOrthof
#define OPENGLES_1_1
#elif defined (MACINTOSH)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#elif defined (WINDOWS_STORE)
#include <d3d11_1.h>
#include <DirectXMath.h>
AT_API ID3D11Device1* ATRender_GetDevice();
AT_API ID3D11DeviceContext1* ATRender_GetContext();
#elif defined (ANDROID)
#include <EGL/egl.h>
#include <GLES/gl.h>
#define OPENGLES_1_1
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#endif //AT_RENDER_INTERNAL_H
