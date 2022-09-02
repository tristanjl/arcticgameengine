#include "AT.h"
#include "ATTime.h"
#include "ATArray.h"
#include "ATSystem.h"

typedef void (*ATSystem_SystemUpdateFunction)(void);

ATSystemCallback g_SystemCallbacks[NUMBER_SYSTEM_CALLBACKS] = {0};
static int64 s_CurrentTime;
static int64 s_PreviousTime;
static bool s_FrameReady;
static bool s_SystemActive;
static bool s_SystemSuspended;
uint32 s_ActiveSystems;

ATArray<ATSystem_SystemUpdateFunction>* g_SystemUpdates;

AT_API void ATSystem_Init(ATSystemInitData* data)
{
	g_SystemUpdates = ATNew(ATArray<ATSystem_SystemUpdateFunction>);

	s_ActiveSystems = data->activeSystems;

	ATLOG(ATDLL_Information, "Initializing Platform");
	void ATSystem_InitPlatformSpecific(ATSystemInitData* data); ATSystem_InitPlatformSpecific(data);

	s_CurrentTime = ATTime_GetSystemTimeInMicroseconds();
	s_PreviousTime = s_CurrentTime;
	s_FrameReady = false;
	s_SystemActive = true;
	s_SystemSuspended = false;
}

AT_API void ATSystem_Deinit()
{
	void ATSystem_DeinitPlatformSpecific(); ATSystem_DeinitPlatformSpecific();

	ATDelete(ATArray<ATSystem_SystemUpdateFunction>, g_SystemUpdates);

	void ATMemory_VerifyMemory(); ATMemory_VerifyMemory();
}

AT_API void ATSystem_RegisterCallback(ATSystemCallbacks callbackType, ATSystemCallback callback)
{
	g_SystemCallbacks[callbackType] = callback;
}

AT_API void ATSystem_InitialiseSystems()
{
	ATLOG(ATDLL_Information, "Initializing Systems");
	if (s_ActiveSystems & ATS_Input)
	{
		void ATInput_Init(); ATInput_Init();
		void ATInput_Update();
		void ATAudio_Update();
		(*g_SystemUpdates).Add(&ATInput_Update);
		(*g_SystemUpdates).Add(&ATAudio_Update);
	}
	if (s_ActiveSystems & ATS_Render)
	{
		void ATRender_Init(); ATRender_Init();
		void ATBasicFont_Init(); ATBasicFont_Init();
	}
	if (s_ActiveSystems & ATS_Audio)
	{
		void ATAudio_Init(); ATAudio_Init();
	}
	if (s_ActiveSystems & ATS_UI)
	{
		void ATUI_Init(); ATUI_Init();
	}

	if (g_SystemCallbacks[ATSC_Init])
	{
		g_SystemCallbacks[ATSC_Init]();
	}
}

AT_API void ATSystem_DeinitialiseSystems()
{
	ATASSERT(g_SystemCallbacks[ATSC_Deinit] != 0, "ATSystem: No Deinit function specified");
	g_SystemCallbacks[ATSC_Deinit]();

	if (s_ActiveSystems & ATS_UI)
	{
		void ATUI_Deinit(); ATUI_Deinit();
	}
	if (s_ActiveSystems & ATS_Render)
	{
		void ATBasicFont_Deinit(); ATBasicFont_Deinit();
		void ATRender_Deinit(); ATRender_Deinit();
	}
	if (s_ActiveSystems & ATS_Audio)
	{
		void ATAudio_Deinit(); ATAudio_Deinit();
	}
	if (s_ActiveSystems & ATS_Input)
	{
		void ATInput_Deinit(); ATInput_Deinit();
	}
}

AT_API void ATSystem_UpdateInternalSystems()
{
	for (size_t i = 0; i < (*g_SystemUpdates).Count(); ++i)
	{
		(*g_SystemUpdates)[i]();
	}
}

AT_API uint32 ATPrimitive_VertexCount();

AT_API void ATSystem_Render()
{
	void ATSprite_Flush(); ATSprite_Flush();

	if (ATPrimitive_VertexCount() > 0)
	{
		void ATPrimitive_Flush(); ATPrimitive_Flush();
	}
	void ATSystem_DisplayScreen(); ATSystem_DisplayScreen();
}

AT_API bool ATSystem_Update()
{
	if (!ATSystem_Suspended() && ATTime_GetSystemTimeInMicroseconds() - s_PreviousTime > ATTime_GetFrameRateInMicroseconds())
	{
		s_PreviousTime = s_CurrentTime;
		s_CurrentTime += ATTime_GetFrameRateInMicroseconds();

		ATSystem_UpdateInternalSystems();

		if (!g_SystemCallbacks[ATSC_Update]() || !g_SystemCallbacks[ATSC_Render]())
		{
			void ATSystem_Shutdown(); ATSystem_Shutdown();
			return false;
		}

		if (s_ActiveSystems & ATS_Render)
		{
			ATSystem_Render();
		}
	}
	return true;
}

AT_API bool ATSystem_Suspended()
{
	return s_SystemSuspended;
}

AT_API void ATSystem_Suspend()
{
	s_SystemSuspended = true;
}

AT_API void ATSystem_Resume()
{
	s_SystemSuspended = false;
}


AT_API bool ATSystem_Active()
{
	return s_SystemActive;
}

AT_API void ATSystem_Shutdown()
{
	s_SystemActive = false;
}
