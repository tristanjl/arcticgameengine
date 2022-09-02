#ifndef AT_INPUT_H
#define AT_INPUT_H

enum ATDevices
{
	ATD_Gamepad1,
	ATD_Gamepad2,
	ATD_Gamepad3,
	ATD_Gamepad4,
	ATD_Gamepad5,
	ATD_Gamepad6,
	ATD_Gamepad7,
	ATD_Gamepad8,

	ATD_GAMEPAD_MAX,

	ATD_XDevice1 = ATD_Gamepad1,
	ATD_XDevice2 = ATD_Gamepad2,
	ATD_XDevice3 = ATD_Gamepad3,
	ATD_XDevice4 = ATD_Gamepad4,

	ATD_Keyboard = ATD_GAMEPAD_MAX,
	ATD_KEYBOARD_MAX
};

enum ATGamepadInputMapping
{
	ATGIM_XAxis1,
	ATGIM_YAxis1,
	ATGIM_XAxis2,
	ATGIM_YAxis2,
	ATGIM_DPadUp,
	ATGIM_DPadDown,
	ATGIM_DPadLeft,
	ATGIM_DPadRight,
	ATGIM_Button1, // Xbox A; Nintendo B; PS Cross; OUYA O
	ATGIM_Button2, // Xbox B; Nintendo A; PS Circle; OUYA A
	ATGIM_Button3, // Xbox X; Nintendo Y; PS Square; OUYA U
	ATGIM_Button4, // Xbox Y; Nintendo X; PS Triangle; OUYA Y
	ATGIM_Menu1, // Xbox Start; Nintendo Start; PS Start
	ATGIM_Menu2, // Xbox Back; Nintendo Select; PS Select
	ATGIM_L1,
	ATGIM_L2,
	ATGIM_R1,
	ATGIM_R2,
	ATGIM_LeftAxisButton,
	ATGIM_RightAxisButton,

	ATGIM_MAX
};

enum ATKeyboardInputMapping
{
	ATKIM_Backspace,
	ATKIM_Tab,
	ATKIM_Return,
	ATKIM_Shift,
	ATKIM_LeftShift,
	ATKIM_RightShift,
	ATKIM_Control,
	ATKIM_LeftControl,
	ATKIM_RightControl,
	ATKIM_Alt,
	ATKIM_LeftAlt,
	ATKIM_RightAlt,
	ATKIM_OS,
	ATKIM_OSLeft,
	ATKIM_OSRight,
	ATKIM_Pause,
	ATKIM_CapsLock,
	ATKIM_Escape,
	ATKIM_Space,
	ATKIM_PageUp,
	ATKIM_PageDown,
	ATKIM_End,
	ATKIM_Home,
	ATKIM_Left,
	ATKIM_Up,
	ATKIM_Right,
	ATKIM_Down,
	ATKIM_PrintScreen,
	ATKIM_Insert,
	ATKIM_Delete,
	ATKIM_Numpad0,
	ATKIM_Numpad1,
	ATKIM_Numpad2,
	ATKIM_Numpad3,
	ATKIM_Numpad4,
	ATKIM_Numpad5,
	ATKIM_Numpad6,
	ATKIM_Numpad7,
	ATKIM_Numpad8,
	ATKIM_Numpad9,
	ATKIM_Multiply,
	ATKIM_Add,
	ATKIM_Subtract,
	ATKIM_Decimal,
	ATKIM_Divide,
	ATKIM_NumLock,
	ATKIM_F1,
	ATKIM_F2,
	ATKIM_F3,
	ATKIM_F4,
	ATKIM_F5,
	ATKIM_F6,
	ATKIM_F7,
	ATKIM_F8,
	ATKIM_F9,
	ATKIM_F10,
	ATKIM_F11,
	ATKIM_F12,
	ATKIM_ScrollLock,
	ATKIM_0,
	ATKIM_1,
	ATKIM_2,
	ATKIM_3,
	ATKIM_4,
	ATKIM_5,
	ATKIM_6,
	ATKIM_7,
	ATKIM_8,
	ATKIM_9,
	ATKIM_A,
	ATKIM_B,
	ATKIM_C,
	ATKIM_D,
	ATKIM_E,
	ATKIM_F,
	ATKIM_G,
	ATKIM_H,
	ATKIM_I,
	ATKIM_J,
	ATKIM_K,
	ATKIM_L,
	ATKIM_M,
	ATKIM_N,
	ATKIM_O,
	ATKIM_P,
	ATKIM_Q,
	ATKIM_R,
	ATKIM_S,
	ATKIM_T,
	ATKIM_U,
	ATKIM_V,
	ATKIM_W,
	ATKIM_X,
	ATKIM_Y,
	ATKIM_Z,
	ATKIM_MAX
};

AT_API bool ATInput_IsDeviceActive(ATDevices device);

AT_API float ATInput_GetState(ATDevices device, int mapping);

AT_API bool ATInput_IsDown(ATDevices device, int mapping);

AT_API size_t ATInput_GetActiveGamepadCount();

#endif // AT_INPUT_H
