#include "AT.h"
#include "ATMath.h"
#include "ATInput_Internal.h"
#include "Android/ATInput_Android_Internal.h"

#include "android/input.h"
#include <dlfcn.h>

static const float ANDROID_DEADZONE = 0.25f;

typedef float(*AndroidGetAxesType)(const AInputEvent*, int32_t axis, size_t pointer_index);
static AndroidGetAxesType s_AMotionEvent_getAxisValue = NULL;

AT_API void ATInput_InitPlatformSpecific()
{
#ifdef ENABLE_OUYA_SUPPORT_ANDROID
	ATInput_InitOUYA();
#endif
#ifdef ENABLE_MOGA_SUPPORT_ANDROID
	ATInput_InitMOGA();
#endif

	void* lib = dlopen("libandroid.so", 0);
	if (lib)
	{
		s_AMotionEvent_getAxisValue = (AndroidGetAxesType)dlsym(lib,"AMotionEvent_getAxisValue");
		ATASSERT(s_AMotionEvent_getAxisValue != NULL, "Controller axis support not available");
	}
}

AT_API void ATInput_DeinitPlatformSpecific()
{
#ifdef ENABLE_OUYA_SUPPORT_ANDROID
	ATInput_DeinitOUYA();
#endif
#ifdef ENABLE_MOGA_SUPPORT_ANDROID
	ATInput_DeinitMOGA();
#endif
}

AT_API void ATInput_UpdatePlatformSpecific()
{
#ifdef ENABLE_OUYA_SUPPORT_ANDROID
	ATInput_UpdateOUYA();
#endif
#ifdef ENABLE_MOGA_SUPPORT_ANDROID
	ATInput_UpdateMOGA();
#endif
}

AT_API size_t ATInput_GetGamepadIndexByDeviceId(int32_t deviceId)
{
#ifdef ENABLE_OUYA_SUPPORT_ANDROID
	return ATInput_GetGamepadIndexByDeviceIdOUYA(deviceId);
#else
	return 0;
#endif
}

AT_API int ATInput_HandleNativeInput(AInputEvent* event)
{
	int32 eventSource = AInputEvent_getSource(event);
	if ((eventSource & AINPUT_SOURCE_GAMEPAD) || (eventSource & AINPUT_SOURCE_JOYSTICK))
	{
		int32_t deviceId = AInputEvent_getDeviceId(event);

		GamepadData* const data = &s_GamepadData[ATInput_GetGamepadIndexByDeviceId(deviceId)];

		//We only handle motion events (touchscreen, axis) and key (button/key) events
		int32_t eventType = AInputEvent_getType(event);

		if (eventType == AINPUT_EVENT_TYPE_MOTION)
		{
			if (AInputEvent_getSource(event) == AINPUT_SOURCE_JOYSTICK)
			{
				float lx = s_AMotionEvent_getAxisValue(event, 0, 0);
				float ly = -s_AMotionEvent_getAxisValue(event, 1, 0);
				float rx = s_AMotionEvent_getAxisValue(event, 2, 0);
				float ry = -s_AMotionEvent_getAxisValue(event, 3, 0);

				float magnitude = ATMath_SqaureRoot((lx * lx) + (ly * ly));
				if (magnitude < ANDROID_DEADZONE)
				{
					data->inputStates[ATGIM_XAxis1] = 0.0f;
					data->inputStates[ATGIM_YAxis1] = 0.0f;
				}
				else
				{
					data->inputStates[ATGIM_XAxis1] = lx * (magnitude - ANDROID_DEADZONE) / (1.0f - ANDROID_DEADZONE);
					data->inputStates[ATGIM_YAxis1] = ly * (magnitude - ANDROID_DEADZONE) / (1.0f - ANDROID_DEADZONE);
				}

				magnitude = ATMath_SqaureRoot((rx * rx) + (ry * ry));
				if (magnitude < ANDROID_DEADZONE)
				{
					data->inputStates[ATGIM_XAxis2] = 0.0f;
					data->inputStates[ATGIM_YAxis2] = 0.0f;
				}
				else
				{
					data->inputStates[ATGIM_XAxis2] = rx * (magnitude - ANDROID_DEADZONE) / (1.0f - ANDROID_DEADZONE);
					data->inputStates[ATGIM_YAxis2] = ry * (magnitude - ANDROID_DEADZONE) / (1.0f - ANDROID_DEADZONE);
				}
			}
			else
			{
				// Ignoring touch input
			}

			return 1;
		}
		else if (eventType == AINPUT_EVENT_TYPE_KEY)
		{
			int32_t code = AKeyEvent_getKeyCode((const AInputEvent*)event);
			int32_t action = AKeyEvent_getAction((const AInputEvent*)event);
			float buttonValue;
			if (action == AKEY_EVENT_ACTION_DOWN)
			{
				buttonValue = 1.0f;
			}
			else if (action == AKEY_EVENT_ACTION_UP)
			{
				buttonValue = 0.0f;
			}
			else
			{
				return 0;
			}

			switch (code)
			{
			case AKEYCODE_DPAD_UP: data->inputStates[ATGIM_DPadUp] = buttonValue; return 1;
			case AKEYCODE_DPAD_DOWN: data->inputStates[ATGIM_DPadDown] = buttonValue; return 1;
			case AKEYCODE_DPAD_LEFT: data->inputStates[ATGIM_DPadLeft] = buttonValue; return 1;
			case AKEYCODE_DPAD_RIGHT: data->inputStates[ATGIM_DPadRight] = buttonValue; return 1;
			case AKEYCODE_BUTTON_A: data->inputStates[ATGIM_Button1] = buttonValue; return 1;
			case AKEYCODE_BUTTON_B: data->inputStates[ATGIM_Button2] = buttonValue; return 1;
			case AKEYCODE_BUTTON_X: data->inputStates[ATGIM_Button3] = buttonValue; return 1;
			case AKEYCODE_BUTTON_Y: data->inputStates[ATGIM_Button4] = buttonValue; return 1;
			case AKEYCODE_BUTTON_START:  data->inputStates[ATGIM_Menu1] = buttonValue; return 1;
			case AKEYCODE_BUTTON_SELECT: data->inputStates[ATGIM_Menu2] = buttonValue; return 1;
			case AKEYCODE_BUTTON_L1: data->inputStates[ATGIM_L1] = buttonValue; return 1;
			case AKEYCODE_BUTTON_L2: data->inputStates[ATGIM_L2] = buttonValue; return 1;
			case AKEYCODE_BUTTON_R1: data->inputStates[ATGIM_R1] = buttonValue; return 1;
			case AKEYCODE_BUTTON_R2: data->inputStates[ATGIM_R2] = buttonValue; return 1;
			case AKEYCODE_BUTTON_THUMBR: data->inputStates[ATGIM_RightAxisButton] = buttonValue; return 1;
			case AKEYCODE_BUTTON_THUMBL: data->inputStates[ATGIM_LeftAxisButton] = buttonValue; return 1;
			}
		}
	}
	if ((eventSource & AINPUT_SOURCE_KEYBOARD))
	{
		KeyboardData* const data = &s_KeyboardData[0];
		int32_t code = AKeyEvent_getKeyCode((const AInputEvent*)event);
		int32_t action = AKeyEvent_getAction((const AInputEvent*)event);
		float keyValue;
		if (action == AKEY_EVENT_ACTION_DOWN)
		{
			keyValue = 1.0f;
		}
		else if (action == AKEY_EVENT_ACTION_UP)
		{
			keyValue = 0.0f;
		}
		else
		{
			return 0;
		}

		if (code >= AKEYCODE_0 && code <= AKEYCODE_9)
		{
			data->inputStates[ATKIM_0 + code - AKEYCODE_0] = keyValue;
		}
		if (code >= AKEYCODE_A && code <= AKEYCODE_Z)
		{
			data->inputStates[ATKIM_A + code - AKEYCODE_A] = keyValue;
		}
		if (code >= AKEYCODE_F1 && code <= AKEYCODE_F12)
		{
			data->inputStates[ATKIM_F1 + code - AKEYCODE_F1] = keyValue;
		}
		if (code >= AKEYCODE_NUMPAD_0 && code <= AKEYCODE_NUMPAD_9)
		{
			data->inputStates[ATKIM_Numpad0 + code - AKEYCODE_NUMPAD_0] = keyValue;
		}

		switch (code)
		{
		case AKEYCODE_BACK: s_KeyboardData[0].inputStates[ATKIM_Backspace] = keyValue; return 1;
		case AKEYCODE_TAB: s_KeyboardData[0].inputStates[ATKIM_Tab] = keyValue; return 1;
		case AKEYCODE_ENTER: s_KeyboardData[0].inputStates[ATKIM_Return] = keyValue; return 1;
		case AKEYCODE_SHIFT_LEFT: s_KeyboardData[0].inputStates[ATKIM_Shift] = keyValue; s_KeyboardData[0].inputStates[ATKIM_LeftShift] = keyValue; return 1;
		case AKEYCODE_SHIFT_RIGHT: s_KeyboardData[0].inputStates[ATKIM_Shift] = keyValue; s_KeyboardData[0].inputStates[ATKIM_RightShift] = keyValue; return 1;
		case AKEYCODE_CTRL_LEFT: s_KeyboardData[0].inputStates[ATKIM_Control] = keyValue; s_KeyboardData[0].inputStates[ATKIM_LeftControl] = keyValue; return 1;
		case AKEYCODE_CTRL_RIGHT: s_KeyboardData[0].inputStates[ATKIM_Control] = keyValue; s_KeyboardData[0].inputStates[ATKIM_RightControl] = keyValue; return 1;
		case AKEYCODE_ALT_LEFT: s_KeyboardData[0].inputStates[ATKIM_Alt] = keyValue; s_KeyboardData[0].inputStates[ATKIM_LeftAlt] = keyValue; return 1;
		case AKEYCODE_ALT_RIGHT: s_KeyboardData[0].inputStates[ATKIM_Alt] = keyValue; s_KeyboardData[0].inputStates[ATKIM_RightAlt] = keyValue; return 1;
		case AKEYCODE_META_LEFT: s_KeyboardData[0].inputStates[ATKIM_OSLeft] = keyValue; s_KeyboardData[0].inputStates[ATKIM_OS] = keyValue; return 1;
		case AKEYCODE_META_RIGHT: s_KeyboardData[0].inputStates[ATKIM_OSRight] = keyValue; s_KeyboardData[0].inputStates[ATKIM_OS] = keyValue; return 1;
		case AKEYCODE_BREAK: s_KeyboardData[0].inputStates[ATKIM_Pause] = keyValue; return 1;
		case AKEYCODE_CAPS_LOCK: s_KeyboardData[0].inputStates[ATKIM_CapsLock] = keyValue; return 1;
		case AKEYCODE_ESCAPE: s_KeyboardData[0].inputStates[ATKIM_Escape] = keyValue; return 1;
		case AKEYCODE_SPACE: s_KeyboardData[0].inputStates[ATKIM_Space] = keyValue; return 1;
		case AKEYCODE_PAGE_UP: s_KeyboardData[0].inputStates[ATKIM_PageUp] = keyValue; return 1;
		case AKEYCODE_PAGE_DOWN: s_KeyboardData[0].inputStates[ATKIM_PageDown] = keyValue; return 1;
		case AKEYCODE_MOVE_END: s_KeyboardData[0].inputStates[ATKIM_End] = keyValue; return 1;
		case AKEYCODE_MOVE_HOME: s_KeyboardData[0].inputStates[ATKIM_Home] = keyValue; return 1;
		case AKEYCODE_DPAD_LEFT: s_KeyboardData[0].inputStates[ATKIM_Left] = keyValue; return 1;
		case AKEYCODE_DPAD_UP: s_KeyboardData[0].inputStates[ATKIM_Up] = keyValue; return 1;
		case AKEYCODE_DPAD_RIGHT: s_KeyboardData[0].inputStates[ATKIM_Right] = keyValue; return 1;
		case AKEYCODE_DPAD_DOWN: s_KeyboardData[0].inputStates[ATKIM_Down] = keyValue; return 1;
		case AKEYCODE_SYSRQ: s_KeyboardData[0].inputStates[ATKIM_PrintScreen] = keyValue; return 1;
		case AKEYCODE_INSERT: s_KeyboardData[0].inputStates[ATKIM_Insert] = keyValue; return 1;
		case AKEYCODE_DEL: s_KeyboardData[0].inputStates[ATKIM_Delete] = keyValue; return 1;
		case AKEYCODE_NUM_LOCK: s_KeyboardData[0].inputStates[ATKIM_NumLock] = keyValue; return 1;
		case AKEYCODE_NUMPAD_MULTIPLY: s_KeyboardData[0].inputStates[ATKIM_Multiply] = keyValue; return 1;
		case AKEYCODE_NUMPAD_ADD: s_KeyboardData[0].inputStates[ATKIM_Add] = keyValue; return 1;
		case AKEYCODE_NUMPAD_SUBTRACT: s_KeyboardData[0].inputStates[ATKIM_Subtract] = keyValue; return 1;
		case AKEYCODE_NUMPAD_DOT: s_KeyboardData[0].inputStates[ATKIM_Decimal] = keyValue; return 1;
		case AKEYCODE_NUMPAD_DIVIDE: s_KeyboardData[0].inputStates[ATKIM_Divide] = keyValue; return 1;
		case AKEYCODE_SCROLL_LOCK: s_KeyboardData[0].inputStates[ATKIM_ScrollLock] = keyValue; return 1;
		}
	}
	return 0;
}
