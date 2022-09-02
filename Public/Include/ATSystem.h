#ifndef SYSTEM_H
#define SYSTEM_H

#include "ATTypes.h"
#include "ATTargets.h"

#ifdef WINDOWS_GCC
#define WIN32_LEAN_AND_MEAN
#include "w32api/windows.h"
#endif

#ifdef WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#ifdef WINDOWS_STORE
#include <agile.h>
#endif

#ifdef ANDROID
struct android_app;
#endif

struct SDL_Window;

enum ATSystemCallbacks
{
	ATSC_Init,
	ATSC_Update,
	ATSC_Render,
	ATSC_Deinit,
	NUMBER_SYSTEM_CALLBACKS
};

enum ATSystems
{
	ATS_Core = ATBIT(0),
	ATS_Render = ATBIT(1),
	ATS_Input = ATBIT(2),
	ATS_Audio = ATBIT(3),
	ATS_Video = ATBIT(4),
	ATS_UI = ATBIT(5),

	ATS_Default = ATS_Core | ATS_Render | ATS_Input | ATS_Audio | ATS_Video
};

struct ATSystemInitData
{
//#if defined (WINDOWS) || (WINDOWS_GCC)
//	HINSTANCE hInstance;
//	HINSTANCE hPrevInstance;
//	LPSTR lpCmdLine;
//	int nCmdShow;
//#elif defined(WINDOWS_STORE)
//	Platform::Array<Platform::String^>^ commandArgs;
//#elif ANDROID
//	android_app* state;
//#else
	int argc;
	char **argv;
//#endif
	const char* programName;
	int width;
	int height;
	uint32 activeSystems;
};

typedef bool (*ATSystemCallback)(void);

AT_API void ATSystem_RegisterCallback(ATSystemCallbacks callbackType, ATSystemCallback callback);

AT_API void ATSystem_Init(ATSystemInitData* data);

AT_API bool ATSystem_Suspended();

// Function where all the game logic and rendering will take place
AT_API void ATSystem_Main();

AT_API void ATSystem_Deinit();

#endif // SYSTEM_H
