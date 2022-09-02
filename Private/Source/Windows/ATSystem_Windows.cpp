#include "AT.h"
#include "ATInput.h"
#include "ATSystem.h"
#include "ATInput_Internal.h"
#include "ATRender_Internal.h"
#include "ATSystem_Internal.h"
#include "ATInputCursor_Internal.h"

HDC hDC;
HGLRC hRC;
HWND mainHWnd;
extern ATSystemCallback g_SystemCallbacks[];
const char windowClassName[] = "ATMAIN";

static ATRect s_WindowRect;

AT_API const ATRect* ATSystem_GetWindowRect()
{
	return &s_WindowRect;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (!ATSystem_Active())
	{
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	switch (message)
	{
	case WM_CREATE:
		return 0;
	case WM_CLOSE:
	case WM_DESTROY:
		ATSystem_Shutdown();
		return 0;
	case WM_SIZE:
		{
			WORD width = LOWORD(lParam);
			WORD height = LOWORD(lParam);
			glViewport(0, 0, width, height);

			RECT clientRect;
			GetClientRect(mainHWnd, &clientRect);

			int frameSize = GetSystemMetrics(SM_CYCAPTION);
			int borderYSize = GetSystemMetrics(SM_CYBORDER);
			int borderXSize = GetSystemMetrics(SM_CXBORDER);

			s_WindowRect.x = (float)(clientRect.left - borderXSize);
			s_WindowRect.y = (float)(clientRect.bottom - clientRect.top - frameSize * 2);
			s_WindowRect.width = (float)(width - borderXSize * 2);
			s_WindowRect.height = (float)(-height + frameSize + borderYSize);
		}
		return 0;
	case WM_KEYDOWN:
	case WM_KEYUP:
		{
			float keyValue = message == WM_KEYDOWN ? 1.0f : 0.0f;
			if (wParam >= '0' && wParam <= '9')
			{
				s_KeyboardData[0].inputStates[ATKIM_0 + (wParam - '0')] = keyValue;
			}
			if (wParam >= 'A' && wParam <= 'Z')
			{
				s_KeyboardData[0].inputStates[ATKIM_A + (wParam - 'A')] = keyValue;
			}
			if (wParam >= VK_NUMPAD0 && wParam <= VK_NUMPAD9)
			{
				s_KeyboardData[0].inputStates[ATKIM_Numpad0 + (wParam - VK_NUMPAD0)] = keyValue;
			}
			if (wParam >= VK_F1 && wParam <= VK_F12)
			{
				s_KeyboardData[0].inputStates[ATKIM_F1 + (wParam - VK_F1)] = keyValue;
			}
			switch (wParam)
			{
			case VK_BACK: s_KeyboardData[0].inputStates[ATKIM_Backspace] = keyValue; return 0;
			case VK_TAB: s_KeyboardData[0].inputStates[ATKIM_Tab] = keyValue; return 0;
			case VK_RETURN: s_KeyboardData[0].inputStates[ATKIM_Return] = keyValue; return 0;
			case VK_SHIFT: s_KeyboardData[0].inputStates[ATKIM_Shift] = keyValue; return 0;
			case VK_LSHIFT: s_KeyboardData[0].inputStates[ATKIM_LeftShift] = keyValue; return 0;
			case VK_RSHIFT: s_KeyboardData[0].inputStates[ATKIM_RightShift] = keyValue; return 0;
			case VK_CONTROL: s_KeyboardData[0].inputStates[ATKIM_Control] = keyValue; return 0;
			case VK_LCONTROL: s_KeyboardData[0].inputStates[ATKIM_LeftControl] = keyValue; return 0;
			case VK_RCONTROL: s_KeyboardData[0].inputStates[ATKIM_RightControl] = keyValue; return 0;
			case VK_MENU: s_KeyboardData[0].inputStates[ATKIM_Alt] = keyValue; return 0;
			case VK_LMENU: s_KeyboardData[0].inputStates[ATKIM_LeftAlt] = keyValue; return 0;
			case VK_RMENU: s_KeyboardData[0].inputStates[ATKIM_RightAlt] = keyValue; return 0;
			case VK_LWIN: s_KeyboardData[0].inputStates[ATKIM_OSLeft] = keyValue; s_KeyboardData[0].inputStates[ATKIM_OS] = keyValue; return 0;
			case VK_RWIN: s_KeyboardData[0].inputStates[ATKIM_OSRight] = keyValue; s_KeyboardData[0].inputStates[ATKIM_OS] = keyValue; return 0;
			case VK_PAUSE: s_KeyboardData[0].inputStates[ATKIM_Pause] = keyValue; return 0;
			case VK_CAPITAL: s_KeyboardData[0].inputStates[ATKIM_CapsLock] = keyValue; return 0;
			case VK_ESCAPE: s_KeyboardData[0].inputStates[ATKIM_Escape] = keyValue; return 0;
			case VK_SPACE: s_KeyboardData[0].inputStates[ATKIM_Space] = keyValue; return 0;
			case VK_PRIOR: s_KeyboardData[0].inputStates[ATKIM_PageUp] = keyValue; return 0;
			case VK_NEXT: s_KeyboardData[0].inputStates[ATKIM_PageDown] = keyValue; return 0;
			case VK_END: s_KeyboardData[0].inputStates[ATKIM_End] = keyValue; return 0;
			case VK_HOME: s_KeyboardData[0].inputStates[ATKIM_Home] = keyValue; return 0;
			case VK_LEFT: s_KeyboardData[0].inputStates[ATKIM_Left] = keyValue; return 0;
			case VK_UP: s_KeyboardData[0].inputStates[ATKIM_Up] = keyValue; return 0;
			case VK_RIGHT: s_KeyboardData[0].inputStates[ATKIM_Right] = keyValue; return 0;
			case VK_DOWN: s_KeyboardData[0].inputStates[ATKIM_Down] = keyValue; return 0;
			case VK_PRINT: s_KeyboardData[0].inputStates[ATKIM_PrintScreen] = keyValue; return 0;
			case VK_INSERT: s_KeyboardData[0].inputStates[ATKIM_Insert] = keyValue; return 0;
			case VK_DELETE: s_KeyboardData[0].inputStates[ATKIM_Delete] = keyValue; return 0;
			case VK_NUMLOCK: s_KeyboardData[0].inputStates[ATKIM_NumLock] = keyValue; return 0;
			case VK_MULTIPLY: s_KeyboardData[0].inputStates[ATKIM_Multiply] = keyValue; return 0;
			case VK_ADD: s_KeyboardData[0].inputStates[ATKIM_Add] = keyValue; return 0;
			case VK_SUBTRACT: s_KeyboardData[0].inputStates[ATKIM_Subtract] = keyValue; return 0;
			case VK_DECIMAL: s_KeyboardData[0].inputStates[ATKIM_Decimal] = keyValue; return 0;
			case VK_DIVIDE: s_KeyboardData[0].inputStates[ATKIM_Divide] = keyValue; return 0;
			case VK_SCROLL: s_KeyboardData[0].inputStates[ATKIM_ScrollLock] = keyValue; return 0;
			}
		}
		return 0;
	case WM_MOUSEMOVE:
		{
			int xPos = LOWORD(lParam);
			int yPos = HIWORD(lParam);
			ATInputCursor_UpdateMouseState(ATIMI_Position_X, 0, (float)xPos);
			ATInputCursor_UpdateMouseState(ATIMI_Position_Y, 0, (float)yPos);
			return 0;
		}
	case WM_LBUTTONDOWN:
		{
			ATInputCursor_UpdateMouseState(ATIMI_Button_1, 0, 1.0f);
			return 0;
		}
	case WM_LBUTTONUP:
		{
			ATInputCursor_UpdateMouseState(ATIMI_Button_1, 0, 0.0f);
			return 0;
		}
	case WM_RBUTTONDOWN:
		{
			ATInputCursor_UpdateMouseState(ATIMI_Button_2, 0, 1.0f);
			return 0;
		}
	case WM_RBUTTONUP:
		{
			ATInputCursor_UpdateMouseState(ATIMI_Button_2, 0, 0.0f);
			return 0;
		}
	case WM_MBUTTONDOWN:
		{
			ATInputCursor_UpdateMouseState(ATIMI_Button_3, 0, 1.0f);
			return 0;
		}
	case WM_MBUTTONUP:
		{
			ATInputCursor_UpdateMouseState(ATIMI_Button_3, 0, 0.0f);
			return 0;
		}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}

AT_API void ATSystem_InitPlatformSpecific(ATSystemInitData* data)
{
	if (data->activeSystems & ATS_Core)
	{
		WNDCLASSEX wc;

		wc.cbSize        = sizeof(WNDCLASSEX);
		wc.style         = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc   = WndProc;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.hInstance     = 0;
		wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
		wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wc.lpszMenuName  = NULL;
		wc.lpszClassName = windowClassName;
		wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

		if (!RegisterClassEx(&wc))
		{
			MessageBox(NULL, "Window Registration Failed!", "Error!",
				MB_ICONEXCLAMATION | MB_OK);
			return;
		}

		RECT windowRect;
		windowRect.left = 0;
		windowRect.top = 0;
		windowRect.right = data->width;
		windowRect.bottom = data->height;
		AdjustWindowRectEx(&windowRect, WS_OVERLAPPEDWINDOW, FALSE, WS_EX_CLIENTEDGE);

		mainHWnd = CreateWindowEx(
			WS_EX_CLIENTEDGE,
			windowClassName,
			data->programName,
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			windowRect.right - windowRect.left,
			windowRect.bottom - windowRect.top,	
			NULL,
			NULL,
			NULL,
			NULL);

		if (!mainHWnd)
			return;

		ShowWindow(mainHWnd, SW_SHOW);
		UpdateWindow(mainHWnd);

		PIXELFORMATDESCRIPTOR pfd;
		int format;

		hDC = GetDC(mainHWnd);

		ZeroMemory(&pfd, sizeof(pfd));
		pfd.nSize = sizeof(pfd);
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 24;
		pfd.cDepthBits = 16;
		pfd.iLayerType = PFD_MAIN_PLANE;
		format = ChoosePixelFormat(hDC, &pfd);
		SetPixelFormat(hDC, format, &pfd);

		hRC = wglCreateContext(hDC);
		wglMakeCurrent(hDC, hRC);

		RECT clientRect;
		GetClientRect(mainHWnd, &clientRect);

		s_WindowRect.x = (float)(clientRect.left);
		s_WindowRect.y = (float)(clientRect.bottom - clientRect.top);
		s_WindowRect.width = (float)(clientRect.right - clientRect.left);
		s_WindowRect.height = (float)(clientRect.top - clientRect.bottom);
	}

	ATSystem_InitialiseSystems();
}

void ATSystem_Main()
{
	MSG msg;

	ATASSERT(g_SystemCallbacks[ATSC_Update] != 0, "ATSystem: No update function specified");
	ATASSERT(g_SystemCallbacks[ATSC_Render] != 0, "ATSystem: No render function specified");

	PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);

	while (ATSystem_Active())
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		ATSystem_Update();
	}
}

AT_API void ATSystem_DisplayScreen()
{
	SwapBuffers(hDC);
}

AT_API void ATSystem_DeinitPlatformSpecific()
{
	ATSystem_DeinitialiseSystems();

	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(hRC);
	ReleaseDC(mainHWnd, hDC);
}
