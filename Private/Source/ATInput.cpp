#include "AT.h"
#include "ATTime.h"

#include "ATInput_Internal.h"

GamepadData* s_GamepadData;
KeyboardData* s_KeyboardData;

AT_API void ATInput_InitPlatformSpecific();
AT_API void ATInput_DeinitPlatformSpecific();

AT_API void ATInput_Init()
{
	void ATInputCursor_Init(); ATInputCursor_Init();

	s_GamepadData = (GamepadData*)ATAlloc(sizeof(GamepadData) * ATD_GAMEPAD_MAX);
	ATMemSet(s_GamepadData, 0, sizeof(GamepadData) * ATD_GAMEPAD_MAX);

	s_KeyboardData = (KeyboardData*)ATAlloc(sizeof(KeyboardData) * ATD_KEYBOARD_MAX - ATD_Keyboard);
	ATMemSet(s_KeyboardData, 0, sizeof(KeyboardData) * ATD_KEYBOARD_MAX - ATD_Keyboard);

	ATInput_InitPlatformSpecific();
}

AT_API void ATInput_Deinit()
{
	void ATInputCursor_Deinit(); ATInputCursor_Deinit();

	ATFree(s_KeyboardData);
	ATFree(s_GamepadData);

	ATInput_DeinitPlatformSpecific();
}

AT_API void ATInput_Update()
{
	void ATInput_UpdatePlatformSpecific(); ATInput_UpdatePlatformSpecific();

	void ATInputCursor_Update(); ATInputCursor_Update();
}

AT_API bool ATInput_IsDeviceActive(ATDevices device)
{
	ATASSERT(device <= ATD_KEYBOARD_MAX, "Invalid device type");
	if (device >= ATD_GAMEPAD_MAX)
	{
		return s_KeyboardData[device - ATD_Keyboard].active;
	}
	return s_GamepadData[device].active;
}

AT_API float ATInput_GetState(ATDevices device, int mapping)
{
	ATASSERT(device <= ATD_KEYBOARD_MAX, "Invalid device type");
	if (device >= ATD_GAMEPAD_MAX)
	{
		return s_KeyboardData[device - ATD_Keyboard].inputStates[mapping];
	}
	return s_GamepadData[device].inputStates[mapping];
}

AT_API bool ATInput_IsDown(ATDevices device, int mapping)
{
	ATASSERT(device <= ATD_KEYBOARD_MAX, "Invalid device type");
	if (device >= ATD_GAMEPAD_MAX)
	{
		return s_KeyboardData[device - ATD_Keyboard].inputStates[mapping] != 0.0f;
	}
	if (mapping < ATGIM_YAxis2)
	{
		return false;
	}
	return s_GamepadData[device].inputStates[mapping] != 0.0f;
}

AT_API size_t ATInput_GetActiveGamepadCount()
{
	size_t count = 0;
	for (size_t i = 0; i < ATD_GAMEPAD_MAX; ++i)
	{
		if (s_GamepadData[i].active)
		{
			++count;
		}
	}
	return count;
}
