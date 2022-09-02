#include "AT.h"
#include "ATTime.h"
#include "ATSystem.h"
#include "ATInput_Internal.h"
#include "ATInputCursor_Internal.h"
#include "ATSystem_Internal.h"
#include <SDL2/SDL.h>

static SDL_Window* s_Window;
static ATRect s_WindowRect;

AT_API const ATRect* ATSystem_GetWindowRect()
{
    return &s_WindowRect;
}

static bool ATSystem_HandleEvent(const SDL_Event& event)
{
	switch (event.type)
	{
	case SDL_QUIT:
		return false;
    case SDL_WINDOWEVENT:
        switch (event.window.event)
        {
        case SDL_WINDOWEVENT_SHOWN:
            break;
        case SDL_WINDOWEVENT_HIDDEN:
            break;
        case SDL_WINDOWEVENT_EXPOSED:
            break;
        case SDL_WINDOWEVENT_MOVED:
            s_WindowRect.x = (float)(event.window.data1);
            s_WindowRect.y = (float)(event.window.data2);
            break;
        case SDL_WINDOWEVENT_RESIZED:
            s_WindowRect.width = (float)(event.window.data1);
            s_WindowRect.height = (float)(event.window.data2);
            break;
        case SDL_WINDOWEVENT_SIZE_CHANGED:
            s_WindowRect.width = (float)(event.window.data1);
            s_WindowRect.height = (float)(event.window.data2);
            break;
        case SDL_WINDOWEVENT_MINIMIZED:
            break;
        case SDL_WINDOWEVENT_MAXIMIZED:
            break;
        case SDL_WINDOWEVENT_RESTORED:
            break;
        case SDL_WINDOWEVENT_ENTER:
            break;
        case SDL_WINDOWEVENT_LEAVE:
            break;
        case SDL_WINDOWEVENT_FOCUS_GAINED:
            break;
        case SDL_WINDOWEVENT_FOCUS_LOST:
            break;
        case SDL_WINDOWEVENT_CLOSE:
            return false;
        default:
            break;
        }
        return true;
	case SDL_KEYDOWN:
	case SDL_KEYUP:
		{
			int keyCode = event.key.keysym.sym;
			float keyValue = event.type == SDL_KEYDOWN ? 1.0f : 0.0f;
			if (keyCode >= '0' && keyCode <= '9')
			{
				s_KeyboardData[0].inputStates[ATKIM_0 + (keyCode - '0')] = keyValue;
			}
			if (keyCode >= 'A' && keyCode <= 'Z')
			{
				s_KeyboardData[0].inputStates[ATKIM_A + (keyCode - 'A')] = keyValue;
			}
            if (keyCode >= SDLK_KP_1 && keyCode <= SDLK_KP_9)
			{
                s_KeyboardData[0].inputStates[ATKIM_Numpad1 + (keyCode - SDLK_KP_1)] = keyValue;
			}
			if (keyCode >= SDLK_F1 && keyCode <= SDLK_F12)
			{
				s_KeyboardData[0].inputStates[ATKIM_F1 + (keyCode - SDLK_F1)] = keyValue;
			}
            switch (keyCode)
            {
            case SDLK_KP_BACKSPACE: s_KeyboardData[0].inputStates[ATKIM_Backspace] = keyValue; return true;
            case SDLK_TAB: s_KeyboardData[0].inputStates[ATKIM_Tab] = keyValue; return true;
            case SDLK_RETURN: s_KeyboardData[0].inputStates[ATKIM_Return] = keyValue; return true;
            case SDLK_LSHIFT: s_KeyboardData[0].inputStates[ATKIM_Shift] = keyValue; s_KeyboardData[0].inputStates[ATKIM_LeftShift] = keyValue; return true;
            case SDLK_RSHIFT: s_KeyboardData[0].inputStates[ATKIM_Shift] = keyValue; s_KeyboardData[0].inputStates[ATKIM_RightShift] = keyValue; return true;
            case SDLK_LCTRL: s_KeyboardData[0].inputStates[ATKIM_Control] = keyValue; s_KeyboardData[0].inputStates[ATKIM_LeftControl] = keyValue; return true;
            case SDLK_RCTRL: s_KeyboardData[0].inputStates[ATKIM_Control] = keyValue; s_KeyboardData[0].inputStates[ATKIM_RightControl] = keyValue; return true;
            case SDLK_LALT: s_KeyboardData[0].inputStates[ATKIM_Alt] = keyValue; s_KeyboardData[0].inputStates[ATKIM_LeftAlt] = keyValue; return true;
            case SDLK_RALT: s_KeyboardData[0].inputStates[ATKIM_Alt] = keyValue; s_KeyboardData[0].inputStates[ATKIM_RightAlt] = keyValue; return true;
            case SDLK_LGUI: s_KeyboardData[0].inputStates[ATKIM_OSLeft] = keyValue; s_KeyboardData[0].inputStates[ATKIM_OS] = keyValue; return true;
            case SDLK_RGUI: s_KeyboardData[0].inputStates[ATKIM_OSRight] = keyValue; s_KeyboardData[0].inputStates[ATKIM_OS] = keyValue; return true;
            case SDLK_PAUSE: s_KeyboardData[0].inputStates[ATKIM_Pause] = keyValue; return true;
            case SDLK_CAPSLOCK: s_KeyboardData[0].inputStates[ATKIM_CapsLock] = keyValue; return true;
            case SDLK_ESCAPE: s_KeyboardData[0].inputStates[ATKIM_Escape] = keyValue; return true;
            case SDLK_SPACE: s_KeyboardData[0].inputStates[ATKIM_Space] = keyValue; return true;
            case SDLK_PAGEUP: s_KeyboardData[0].inputStates[ATKIM_PageUp] = keyValue; return true;
            case SDLK_PAGEDOWN: s_KeyboardData[0].inputStates[ATKIM_PageDown] = keyValue; return true;
            case SDLK_END: s_KeyboardData[0].inputStates[ATKIM_End] = keyValue; return true;
            case SDLK_HOME: s_KeyboardData[0].inputStates[ATKIM_Home] = keyValue; return true;
            case SDLK_LEFT: s_KeyboardData[0].inputStates[ATKIM_Left] = keyValue; return true;
            case SDLK_UP: s_KeyboardData[0].inputStates[ATKIM_Up] = keyValue; return true;
            case SDLK_RIGHT: s_KeyboardData[0].inputStates[ATKIM_Right] = keyValue; return true;
            case SDLK_DOWN: s_KeyboardData[0].inputStates[ATKIM_Down] = keyValue; return true;
            case SDLK_PRINTSCREEN: s_KeyboardData[0].inputStates[ATKIM_PrintScreen] = keyValue; return true;
            case SDLK_INSERT: s_KeyboardData[0].inputStates[ATKIM_Insert] = keyValue; return true;
            case SDLK_DELETE: s_KeyboardData[0].inputStates[ATKIM_Delete] = keyValue; return true;
            case SDLK_NUMLOCKCLEAR: s_KeyboardData[0].inputStates[ATKIM_NumLock] = keyValue; return true;
            case SDLK_KP_MULTIPLY: s_KeyboardData[0].inputStates[ATKIM_Multiply] = keyValue; return true;
            case SDLK_KP_PLUS: s_KeyboardData[0].inputStates[ATKIM_Add] = keyValue; return true;
            case SDLK_KP_MINUS: s_KeyboardData[0].inputStates[ATKIM_Subtract] = keyValue; return true;
            case SDLK_KP_DECIMAL: s_KeyboardData[0].inputStates[ATKIM_Decimal] = keyValue; return true;
            case SDLK_KP_DIVIDE: s_KeyboardData[0].inputStates[ATKIM_Divide] = keyValue; return true;
            case SDLK_SCROLLLOCK: s_KeyboardData[0].inputStates[ATKIM_ScrollLock] = keyValue; return true;
            }
			return true;
		}
	case SDL_MOUSEMOTION:
		{
			ATInputCursor_UpdateMouseState(ATIMI_Position_X, 0, (float)event.motion.x);
			ATInputCursor_UpdateMouseState(ATIMI_Position_Y, 0, (float)event.motion.y);
			return true;
		}
	case SDL_MOUSEBUTTONDOWN:
		{
			ATInputCursor_UpdateMouseState((ATInputMouseInput)(ATIMI_Position_Y + event.button.button), 0, 1.0f);
			return true;
		}
	case SDL_MOUSEBUTTONUP:
		{
			ATInputCursor_UpdateMouseState((ATInputMouseInput)(ATIMI_Position_Y + event.button.button), 0, 0.0f);
			return true;
		}
	case SDL_JOYAXISMOTION:
		{
			ATInputCursor_UpdateMouseState(ATIMI_Button_1, 0, 0.0f);
			return true;
		}
	case SDL_JOYBUTTONDOWN:
	case SDL_JOYBUTTONUP:
		{
			if (event.jbutton.which > ATD_GAMEPAD_MAX)
			{
				return true;
			}
			GamepadData* const data = &s_GamepadData[(size_t)ATD_Gamepad1 + event.jbutton.which];
			data->inputStates[ATGIM_Button1 + event.jbutton.button] = event.type == SDL_JOYBUTTONDOWN ? 1.0f : 0.0f;
			return true;
		}
	}
	return true;
}

#if defined (EMSCRIPTEN) && !defined (ATEDITOR)
#include "Emscripten/emscripten.h"
static void ATSystem_UpdateIteration()
{
	SDL_Event event;
	SDL_PollEvent(&event);

	if (!ATSystem_HandleEvent(event))
	{
		ATSystem_Deinit();
		return;
	}

	ATSystem_Update();
}
#endif

AT_API void ATSystem_InitPlatformSpecific(ATSystemInitData &data)
{
	SDL_Init(SDL_INIT_VIDEO);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);

    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);


    SDL_GL_SetSwapInterval(1);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

    SDL_Surface* screenSurface = nullptr;

    s_Window = SDL_CreateWindow(data.programName,
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        data.width, data.height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

    SDL_GLContext glContext = SDL_GL_CreateContext(s_Window);
    if (glContext == NULL)
    {
        printf("There was an error creating the OpenGL context!\n");
        return;
    }

    // Create a double-buffered draw context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    screenSurface = SDL_GetWindowSurface(s_Window);

    s_WindowRect.x = 0;
    s_WindowRect.y = 0;
    s_WindowRect.width = screenSurface->w;
    s_WindowRect.height = screenSurface->h;

	ATSystem_InitialiseSystems();

#if defined (EMSCRIPTEN) && !defined (ATEDITOR)
	emscripten_set_main_loop(ATSystem_UpdateIteration, 0, 1);
#endif
}

AT_API void ATSystem_Main()
{
	SDL_Event event;

	while (ATSystem_HandleEvent(event))
	{
		SDL_PollEvent(&event);

		ATSystem_Update();
	}
}

AT_API void ATSystem_DisplayScreen()
{
    SDL_GL_SwapWindow(s_Window);
}

AT_API void ATSystem_DeinitPlatformSpecific()
{
    SDL_DestroyWindow(s_Window);
    s_Window = nullptr;

	SDL_Quit();
}
