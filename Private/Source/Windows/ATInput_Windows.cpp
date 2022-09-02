#include "AT.h"
#include "ATMath.h"
#include "ATInput.h"
#include "ATInput_Internal.h"

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <XInput.h>

#pragma comment(lib, "XInput9_1_0.lib")

static const float XINPUT_DEADZONE = 0.25f;
static const DWORD MAX_XINPUT_CONTROLLERS = 4;

AT_API void ATInput_InitPlatformSpecific()
{
	s_KeyboardData[0].active =  true;
}

AT_API void ATInput_DeinitPlatformSpecific()
{
}

AT_API void ATInput_UpdatePlatformSpecific()
{
	DWORD dwResult;
	for (DWORD i = 0; i < MAX_XINPUT_CONTROLLERS; i++)
	{
		XINPUT_STATE state;
		ATMemSet(&state, 0, sizeof(XINPUT_STATE));
		dwResult = XInputGetState(i, &state);

		GamepadData* const data = &s_GamepadData[(size_t)ATD_XDevice1 + i];

		ATMemCopy(data->previousInputStates, data->inputStates, sizeof(data->previousInputStates));

		if (dwResult == ERROR_SUCCESS)
		{
			data->active = true;
			float lx = ATMath_Clamp(-1.0f, (float)state.Gamepad.sThumbLX / 32767.0f, 1.0f);
			float ly = ATMath_Clamp(-1.0f, (float)state.Gamepad.sThumbLY / 32767.0f, 1.0f);
			float rx = ATMath_Clamp(-1.0f, (float)state.Gamepad.sThumbRX / 32767.0f, 1.0f);
			float ry = ATMath_Clamp(-1.0f, (float)state.Gamepad.sThumbRY / 32767.0f, 1.0f);

			float magnitude = ATMath_SqaureRoot((lx * lx) + (ly * ly));
			if (magnitude < XINPUT_DEADZONE)
			{
				data->inputStates[ATGIM_XAxis1] = 0.0f;
				data->inputStates[ATGIM_YAxis1] = 0.0f;
			}
			else
			{
				data->inputStates[ATGIM_XAxis1] = lx * (magnitude - XINPUT_DEADZONE) / (1.0f - XINPUT_DEADZONE);
				data->inputStates[ATGIM_YAxis1] = ly * (magnitude - XINPUT_DEADZONE) / (1.0f - XINPUT_DEADZONE);
			}

			magnitude = ATMath_SqaureRoot((rx * rx) + (ry * ry));
			if (magnitude < XINPUT_DEADZONE)
			{
				data->inputStates[ATGIM_XAxis2] = 0.0f;
				data->inputStates[ATGIM_YAxis2] = 0.0f;
			}
			else
			{
				data->inputStates[ATGIM_XAxis2] = rx * (magnitude - XINPUT_DEADZONE) / (1.0f - XINPUT_DEADZONE);
				data->inputStates[ATGIM_YAxis2] = ry * (magnitude - XINPUT_DEADZONE) / (1.0f - XINPUT_DEADZONE);
			}

			data->inputStates[ATGIM_DPadUp] = state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP ? 1.0f : 0.0f;
			data->inputStates[ATGIM_DPadDown] = state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN ? 1.0f : 0.0f;
			data->inputStates[ATGIM_DPadLeft] = state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT ? 1.0f : 0.0f;
			data->inputStates[ATGIM_DPadRight] = state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT ? 1.0f : 0.0f;
			data->inputStates[ATGIM_Button1] = state.Gamepad.wButtons & XINPUT_GAMEPAD_A ? 1.0f : 0.0f;
			data->inputStates[ATGIM_Button2] = state.Gamepad.wButtons & XINPUT_GAMEPAD_B ? 1.0f : 0.0f;
			data->inputStates[ATGIM_Button3] = state.Gamepad.wButtons & XINPUT_GAMEPAD_X ? 1.0f : 0.0f;
			data->inputStates[ATGIM_Button4] = state.Gamepad.wButtons & XINPUT_GAMEPAD_Y ? 1.0f : 0.0f;
			data->inputStates[ATGIM_Menu1] = state.Gamepad.wButtons & XINPUT_GAMEPAD_START ? 1.0f : 0.0f;
			data->inputStates[ATGIM_Menu2] = state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK ? 1.0f : 0.0f;
			data->inputStates[ATGIM_L1] = state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER ? 1.0f : 0.0f;
			data->inputStates[ATGIM_R1] = state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER ? 1.0f : 0.0f;
			data->inputStates[ATGIM_LeftAxisButton] = state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB ? 1.0f : 0.0f;
			data->inputStates[ATGIM_RightAxisButton] = state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB ? 1.0f : 0.0f;

			data->inputStates[ATGIM_L2] = state.Gamepad.bLeftTrigger > 30 ? (float)state.Gamepad.bLeftTrigger / 255.0f : 0.0f;
			data->inputStates[ATGIM_R2] = state.Gamepad.bRightTrigger > 30 ? (float)state.Gamepad.bRightTrigger / 255.0f : 0.0f;
		}
		else
		{
			data->active = false;
			ATMemSet(data->inputStates, 0, sizeof(data->inputStates));
		}
	}

	for (int i = 0; i < ATD_KEYBOARD_MAX - ATD_Keyboard; ++i)
	{
		KeyboardData* const data = &s_KeyboardData[i];
		ATMemCopy(data->previousInputStates, data->inputStates, sizeof(data->previousInputStates));
	}
}
