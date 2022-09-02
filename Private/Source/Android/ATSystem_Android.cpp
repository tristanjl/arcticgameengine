#include "AT.h"
#include "ATSystem.h"
#include "ATSystem_Internal.h"
#include "ATRender_Internal.h"

#include "Android/android_native_app_glue.h"

static android_app* s_AndroidAppState = NULL;
static EGLDisplay s_EGLDisplay = NULL;
static EGLSurface s_EGLSurface = NULL;
static EGLContext s_EGLContext = NULL;

AT_API int ATInput_HandleNativeInput(AInputEvent* event);

static bool ATSystem_InitDisplayAndroid(android_app* app)
{
	if (!app || !app->window)
	{
		return false;
	}

	const EGLint attribs[] =
	{
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_BLUE_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_RED_SIZE, 8,
		EGL_NONE
	};

	EGLint format;
	EGLint numConfigs;
	EGLConfig config;

	s_EGLDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);

	eglInitialize(s_EGLDisplay, 0, 0);

	eglChooseConfig(s_EGLDisplay, attribs, &config, 1, &numConfigs);
	eglGetConfigAttrib(s_EGLDisplay, config, EGL_NATIVE_VISUAL_ID, &format);

	ANativeWindow_setBuffersGeometry(app->window, 0, 0, format);

	s_EGLSurface = eglCreateWindowSurface(s_EGLDisplay, config, app->window, NULL);
	s_EGLContext = eglCreateContext(s_EGLDisplay, config, NULL, NULL);

	if (eglMakeCurrent(s_EGLDisplay, s_EGLSurface, s_EGLSurface, s_EGLContext) == EGL_FALSE)
	{
		ATASSERT(false, "Unable to eglMakeCurrent");
		return false;
	}

	return true;
}

static void ATSystem_DeinitDisplayAndroid(android_app* app)
{
	if (s_EGLDisplay != EGL_NO_DISPLAY)
	{
		eglMakeCurrent(s_EGLDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		if (s_EGLContext != EGL_NO_CONTEXT)
		{
			eglDestroyContext(s_EGLDisplay, s_EGLContext);
		}
		if (s_EGLSurface != EGL_NO_SURFACE)
		{
			eglDestroySurface(s_EGLDisplay, s_EGLSurface);
		}
		eglTerminate(s_EGLDisplay);
	}
	s_EGLDisplay = EGL_NO_DISPLAY;
	s_EGLContext = EGL_NO_CONTEXT;
	s_EGLSurface = EGL_NO_SURFACE;
}

static int32_t handleInput(android_app* /*app*/, AInputEvent* event) 
{
	return ATInput_HandleNativeInput(event);
}

static void handleCmd(android_app* app, int32_t cmd)
{
	switch (cmd) 
	{
	case APP_CMD_SAVE_STATE:
		break;
	case APP_CMD_INIT_WINDOW:
		ATSystem_InitDisplayAndroid(app);
		break;
	case APP_CMD_TERM_WINDOW:
		ATSystem_DeinitDisplayAndroid(app);
		break;
	case APP_CMD_GAINED_FOCUS:
		ATSystem_Resume();
		break;
	case APP_CMD_LOST_FOCUS:
		ATSystem_Suspend();
		break;
	case APP_CMD_CONFIG_CHANGED:
		break;
	}
}

AT_API void ATSystem_InitPlatformSpecific(ATSystemInitData* data)
{
	app_dummy();

	s_AndroidAppState = data->state;
	if (data->activeSystems & ATS_Core)
	{
		data->state->onAppCmd = &handleCmd;
	}
	if (data->activeSystems & ATS_Input)
	{
		data->state->onInputEvent = &handleInput;
	}

	if (data->activeSystems & ATS_Render)
	{
		int ident, events;
		struct android_poll_source* source;
		while (!s_EGLDisplay && (ident = ALooper_pollAll(100, NULL, &events, (void**)&source)) >= 0)
		{
			if (source != NULL)
			{
				source->process(s_AndroidAppState, source);
			}

			if (s_AndroidAppState->destroyRequested != 0)
			{
				ATSystem_Shutdown();
				break;
			}
		}
	}

	ATSystem_InitialiseSystems();
}

AT_API void ATSystem_DeinitPlatformSpecific()
{
	s_AndroidAppState = NULL;
}

void ATSystem_Main()
{
	int ident, events;
	struct android_poll_source* source;

	while (ATSystem_Active())
	{
		while ((ident = ALooper_pollAll(ATSystem_Suspended() ? 250 : 0, NULL, &events, (void**)&source)) >= 0)
		{
			if (source != NULL)
			{
				source->process(s_AndroidAppState, source);
			}

			if (s_AndroidAppState->destroyRequested != 0)
			{
				ATSystem_Shutdown();
				continue;
			}
		}

		ATSystem_Update();
	}
}

AT_API void ATSystem_DisplayScreen()
{
	eglSwapBuffers(s_EGLDisplay, s_EGLSurface);
}

AT_API android_app* ATSystem_Android_GetApp()
{
	return s_AndroidAppState;
}
