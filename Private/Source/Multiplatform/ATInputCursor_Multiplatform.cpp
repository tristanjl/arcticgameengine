#include "AT.h"
#include "ATInputCursor_Internal.h"

#include <SDL2/SDL_mouse.h>

AT_API void ATInputCursor_InitPlatformSpecific()
{
}

AT_API void ATInputCursor_DeinitPlatformSpecific()
{
}

void ATInputCursor_UpdatePlatformSpecific()
{
	int xPosition;
	int yPosition;
	uint8 mouseState = SDL_GetMouseState(&xPosition, &yPosition);

	ATInputCursor_UpdateMouseState(ATIMI_Button_1, 0, SDL_BUTTON(SDL_BUTTON_LEFT)   & mouseState ? 1.0f : 0.0f);
	ATInputCursor_UpdateMouseState(ATIMI_Button_2, 0, SDL_BUTTON(SDL_BUTTON_MIDDLE) & mouseState ? 1.0f : 0.0f);
	ATInputCursor_UpdateMouseState(ATIMI_Button_3, 0, SDL_BUTTON(SDL_BUTTON_RIGHT)  & mouseState ? 1.0f : 0.0f);

	ATInputCursor_UpdateMouseState(ATIMI_Position_X, 0, (float)xPosition);
	ATInputCursor_UpdateMouseState(ATIMI_Position_Y, 0, (float)yPosition);
}
