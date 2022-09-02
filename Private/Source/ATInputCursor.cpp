#include "AT.h"
#include "ATTime.h"
#include "ATMath.h"
#include "ATInputCursor.h"
#include "ATInputCursor_Internal.h"

AT_API const ATRect* ATRender_GetViewportRect();
AT_API const ATRect* ATSystem_GetWindowRect();

const size_t MAX_TOUCH_COUNT = 8;
const size_t MAX_MOUSE_COUNT = 4;

const uint32 INVALID_TOUCH_IDENTIFIER = 0xffffffff;

const float MIN_PERCENTAGE_FRAME_FALLOFF = 0.01f;
const float MAX_PER_FRAME_USAGE_OF_MIN = 5.0f;

#if defined(WINDOWS) || defined(WINDOWS_STORE) || defined(MACINTOSH) || defined(LINUX) || defined(EMSCRIPTEN)
#define SUPPORT_MOUSE_INPUT
#endif

enum ATTouchInputState
{
	ATTIS_Tap,
	ATTIS_Drag
};

struct ATInputTouchData
{
	float currentInputStates[ATITI_MAX];
	float previousInputStates[ATITI_MAX];
	float initialTouchPosition[2];
	float currentTouchVelocity[2];
	float timeActive;
	uint32 touchState;
	uint32 touchIdentifier;
};

#ifdef SUPPORT_MOUSE_INPUT
struct ATInputMouseData
{
	float immediateButtonStates[ATIMI_Mouse_Wheel - ATIMI_Button_1];
	float currentInputStates[ATIMI_MAX];
	float previousInputStates[ATIMI_MAX];
	float timeInState[ATIMI_MAX];
};
#endif

struct ATInputCursorData
{
	ATInputTouchData* touchList;
	uint32 touchCount;

#ifdef SUPPORT_MOUSE_INPUT
	ATInputMouseData* mouseList;
	uint32 mouseCount;
#endif
};

ATInputCursorData *g_CursorData;

AT_API void ATInputCursor_Init()
{
	g_CursorData = (ATInputCursorData*)ATAlloc(sizeof(ATInputCursorData));
	ATMemSet(g_CursorData, 0, sizeof(ATInputCursorData));
	g_CursorData->touchList = (ATInputTouchData*)ATAlloc(sizeof(ATInputTouchData) * MAX_TOUCH_COUNT);
	ATMemSet(g_CursorData->touchList, 0, sizeof(ATInputTouchData) * MAX_TOUCH_COUNT);

#ifdef SUPPORT_MOUSE_INPUT
	g_CursorData->mouseList = (ATInputMouseData*)ATAlloc(sizeof(ATInputMouseData) * MAX_MOUSE_COUNT);
	ATMemSet(g_CursorData->mouseList, 0, sizeof(ATInputMouseData) * MAX_MOUSE_COUNT);
	g_CursorData->mouseCount = MAX_MOUSE_COUNT;
#endif
}

AT_API void ATInputCursor_Deinit()
{
#ifdef SUPPORT_MOUSE_INPUT
	ATFree(g_CursorData->mouseList);
#endif
	ATFree(g_CursorData->touchList);
	ATFree(g_CursorData);
}

AT_API void ATInputCursor_Update()
{
#ifdef SUPPORT_MOUSE_INPUT
	for (size_t i = 0; i < g_CursorData->mouseCount; ++i)
	{
		ATMemCopy(g_CursorData->mouseList[i].previousInputStates, g_CursorData->mouseList[i].currentInputStates, sizeof(g_CursorData->mouseList[i].previousInputStates));
		for (int j = 0; j < ATIMI_MAX; ++j)
		{
			if (g_CursorData->mouseList[i].currentInputStates[j] == g_CursorData->mouseList[i].previousInputStates[j])
			{
				g_CursorData->mouseList[i].timeInState[j] += (float)(((double)ATTime_GetFrameRateInMicroseconds()) / 1000000.0);
			}
			else
			{
				g_CursorData->mouseList[i].timeInState[j] = 0.0f;
			}
		}
	}
#endif

	for (size_t i = 0; i < g_CursorData->touchCount; ++i)
	{
		ATMemCopy(g_CursorData->touchList[i].previousInputStates, g_CursorData->touchList[i].currentInputStates, sizeof(g_CursorData->touchList[i].previousInputStates));

		if (g_CursorData->touchList[i].previousInputStates[ATITI_Active] == 0.0f)
		{
			if (g_CursorData->touchList[i].touchIdentifier != INVALID_TOUCH_IDENTIFIER)
			{
				g_CursorData->touchList[i].touchIdentifier = INVALID_TOUCH_IDENTIFIER;

				g_CursorData->touchList[i].currentTouchVelocity[0] = g_CursorData->touchList[i].currentInputStates[ATITI_Position_X] - g_CursorData->touchList[0].previousInputStates[ATITI_Position_X];
				g_CursorData->touchList[i].currentTouchVelocity[1] = g_CursorData->touchList[i].currentInputStates[ATITI_Position_Y] - g_CursorData->touchList[0].previousInputStates[ATITI_Position_Y];
			}
			else if (g_CursorData->touchList[i].currentTouchVelocity[0] > 0.0f && g_CursorData->touchList[i].currentTouchVelocity[1] > 0.0f)
			{
				float xVel = g_CursorData->touchList[i].currentTouchVelocity[0];
				float yVel = g_CursorData->touchList[i].currentTouchVelocity[1];
				float currentSpeed = ATMath_SqaureRoot(xVel * xVel + yVel * yVel);
				float percentageDecrement = currentSpeed >= MAX_PER_FRAME_USAGE_OF_MIN ? 0.0f : 1 / (MAX_PER_FRAME_USAGE_OF_MIN - currentSpeed);
				percentageDecrement = ATMath_Max(percentageDecrement, MIN_PERCENTAGE_FRAME_FALLOFF);
				float newSpeed = currentSpeed - (percentageDecrement * MAX_PER_FRAME_USAGE_OF_MIN);
				g_CursorData->touchList[i].currentTouchVelocity[0] = xVel * (newSpeed / currentSpeed);
				g_CursorData->touchList[i].currentTouchVelocity[1] = yVel * (newSpeed / currentSpeed);
			}
			else
			{
				--g_CursorData->touchCount;
				if (i != g_CursorData->touchCount)
				{
					ATMemMove(&g_CursorData->touchList[i], &g_CursorData->touchList[i + 1], sizeof(ATInputTouchData) * (g_CursorData->touchCount - i));
				}
			}
		}
	}
}

AT_API void ATInputCursor_UpdateMouseState(ATInputMouseInput mouseInput, int mouseIndex, float inputValue)
{
#ifdef SUPPORT_MOUSE_INPUT
	if (mouseInput <= ATIMI_BUTTON_MAX)
	{
		g_CursorData->mouseList[mouseIndex].immediateButtonStates[mouseInput] = inputValue;
		if (g_CursorData->mouseList[mouseIndex].currentInputStates[mouseInput] != inputValue)
		{
			g_CursorData->mouseList[mouseIndex].currentInputStates[mouseInput] = inputValue;
		}
	}
	else
	{
		g_CursorData->mouseList[mouseIndex].currentInputStates[mouseInput] = inputValue;
	}
#endif
}

AT_API void ATInputCursor_CreateTouchState(uint32 touchIdentifier, float xPosition, float yPosition)
{
	if (g_CursorData->touchCount >= MAX_TOUCH_COUNT)
	{
		return;
	}
	ATMemSet(&g_CursorData->touchList[g_CursorData->touchCount], 0, sizeof(g_CursorData->touchList[g_CursorData->touchCount]));
	g_CursorData->touchList[g_CursorData->touchCount].touchIdentifier = touchIdentifier;
	g_CursorData->touchList[g_CursorData->touchCount].currentInputStates[ATITI_Position_X] = xPosition;
	g_CursorData->touchList[g_CursorData->touchCount].currentInputStates[ATITI_Position_Y] = yPosition;
	g_CursorData->touchList[g_CursorData->touchCount].initialTouchPosition[0] = xPosition;
	g_CursorData->touchList[g_CursorData->touchCount].initialTouchPosition[1] = yPosition;
	g_CursorData->touchList[g_CursorData->touchCount].currentInputStates[ATITI_Active] = 1.0f;
	++g_CursorData->touchCount;
}

AT_API ATInputTouchData* ATInputCursor_GetTouchData
(
#if defined(TRACK_TOUCH_VIA_IDENTIFIER)
    uint32 touchIdentifier
#elif defined(TRACK_TOUCH_VIA_POSITION)
    float positionX, float positionY
#endif
)
{
	for (uint32 i = 0; i < g_CursorData->touchCount; ++i)
	{
#if defined(TRACK_TOUCH_VIA_IDENTIFIER)
		if (g_CursorData->touchList[i].touchIdentifier != touchIdentifier)
		{
			continue;
		}
#elif defined(TRACK_TOUCH_VIA_POSITION)
		if (g_CursorData->touchList[i].currentInputStates[ATITI_Position_X] != positionX ||
            g_CursorData->touchList[i].currentInputStates[ATITI_Position_Y] != positionY)
		{
			continue;
		}
#endif
        return &g_CursorData->touchList[i];
	}
    return nullptr;
}

AT_API void ATInputCursor_UpdateTouchState(ATInputTouchData* data, ATInputTouchInput touchInput, float inputValue)
{
    data->currentInputStates[touchInput] = inputValue;
}

static bool ATInputCursor_GetCursorDelta(ATMouseInputMapping mapping, float* deltaX, float* deltaY)
{
    ATASSERT(!!deltaX && !!deltaY, "Invalid argument - must not be nullptr");

#ifdef SUPPORT_MOUSE_INPUT
	for (uint32 i = 0; i < g_CursorData->mouseCount; ++i)
	{
		switch (mapping)
		{
		case ATMIM_PrimaryMouseClickDrag:
		case ATMIM_SecondaryMouseClickDrag:
		case ATMIM_MiddleMouseClickDrag:
			{
				uint32 mouseButton = (uint32)(mapping + ATIMI_Button_1);
				if (g_CursorData->mouseList[i].currentInputStates[mouseButton] != 1.0f || g_CursorData->mouseList[i].previousInputStates[mouseButton] != 1.0f)
				{
					continue;
				}
				if (g_CursorData->mouseList[i].timeInState[mouseButton] <= g_CursorData->mouseList[i].timeInState[ATIMI_Position_X] &&
					g_CursorData->mouseList[i].timeInState[mouseButton] <= g_CursorData->mouseList[i].timeInState[ATIMI_Position_Y])
				{
					continue;
				}
				*deltaX = g_CursorData->mouseList[i].currentInputStates[ATIMI_Position_X] - g_CursorData->mouseList[i].previousInputStates[ATIMI_Position_X];
				*deltaY = g_CursorData->mouseList[i].currentInputStates[ATIMI_Position_Y] - g_CursorData->mouseList[i].previousInputStates[ATIMI_Position_Y];
			}
			return true;

		case ATMIM_PrimarySecondaryMouseClickDrag:
			if (g_CursorData->mouseList[i].currentInputStates[ATIMI_Button_1] != 1.0f || g_CursorData->mouseList[i].previousInputStates[ATIMI_Button_1] != 1.0f ||
				g_CursorData->mouseList[i].currentInputStates[ATIMI_Button_2] != 1.0f || g_CursorData->mouseList[i].previousInputStates[ATIMI_Button_2] != 1.0f)
			{
				continue;
			}
			if (g_CursorData->mouseList[i].timeInState[ATIMI_Button_1] <= g_CursorData->mouseList[i].timeInState[ATIMI_Position_X] &&
				g_CursorData->mouseList[i].timeInState[ATIMI_Button_1] <= g_CursorData->mouseList[i].timeInState[ATIMI_Position_Y] &&
				g_CursorData->mouseList[i].timeInState[ATIMI_Button_2] <= g_CursorData->mouseList[i].timeInState[ATIMI_Position_X] &&
				g_CursorData->mouseList[i].timeInState[ATIMI_Button_2] <= g_CursorData->mouseList[i].timeInState[ATIMI_Position_Y])
			{
				continue;
			}
			*deltaX = g_CursorData->mouseList[i].currentInputStates[ATIMI_Position_X] - g_CursorData->mouseList[i].previousInputStates[ATIMI_Position_X];
			*deltaY = g_CursorData->mouseList[i].currentInputStates[ATIMI_Position_Y] - g_CursorData->mouseList[i].previousInputStates[ATIMI_Position_Y];
			return true;
		case ATMIM_MouseWheelMove:
			if (g_CursorData->mouseList[i].currentInputStates[ATIMI_Mouse_Wheel] == 0.0f)
			{
				continue;
			}
			*deltaX = g_CursorData->mouseList[i].currentInputStates[ATIMI_Mouse_Wheel];
			*deltaY = *deltaX;
			return true;
			break;
		}
	}
#endif

	return false;
}

AT_API bool ATInputCursor_GetMousePosition(float* xPosition, float* yPosition)
{
#ifdef SUPPORT_MOUSE_INPUT
	const ATRect* windowRect = ATSystem_GetWindowRect();
	const ATRect* viewportRect = ATRender_GetViewportRect();

	*xPosition = viewportRect->x + (g_CursorData->mouseList[0].currentInputStates[ATIMI_Position_X] - windowRect->x) * (viewportRect->width / windowRect->width);
	*yPosition = viewportRect->y + (g_CursorData->mouseList[0].currentInputStates[ATIMI_Position_Y] - windowRect->y) * (viewportRect->height / windowRect->height);
	return true;
#else
	ATUNUSED(xPosition);
	ATUNUSED(yPosition);
	return false;
#endif
}

AT_API bool ATInputCursor_GetTap(float* xPosition, float* yPosition, ATMouseButton button)
{
#ifdef SUPPORT_MOUSE_INPUT
	for (uint32 i = 0; i < g_CursorData->mouseCount; ++i)
	{
		uint32 mouseButton = (uint32)(button + ATIMI_Button_1);
		if (g_CursorData->mouseList[i].currentInputStates[mouseButton] != 1.0f || g_CursorData->mouseList[i].previousInputStates[mouseButton] != 1.0f)
		{
			continue;
		}
		const ATRect* windowRect = ATSystem_GetWindowRect();
		const ATRect* viewportRect = ATRender_GetViewportRect();
		*xPosition = viewportRect->x + (g_CursorData->mouseList[0].currentInputStates[ATIMI_Position_X] - windowRect->x) * (viewportRect->width / windowRect->width);
		*yPosition = viewportRect->y + (g_CursorData->mouseList[0].currentInputStates[ATIMI_Position_Y] - windowRect->y) * (viewportRect->height / windowRect->height);
		return true;
	}
#endif
	if (button != ATMB_Touch)
	{
		return false;
	}

	for (uint32 i = 0; i < g_CursorData->touchCount; ++i)
	{
		if (g_CursorData->touchList[i].previousInputStates[ATITI_Active] == 1.0f)
		{
			continue;
		}
		const ATRect* windowRect = ATSystem_GetWindowRect();
		const ATRect* viewportRect = ATRender_GetViewportRect();
		*xPosition = viewportRect->x + (g_CursorData->touchList[0].currentInputStates[ATITI_Position_X] - windowRect->x) * (viewportRect->width / windowRect->width);
		*yPosition = viewportRect->y + (g_CursorData->touchList[0].currentInputStates[ATITI_Position_Y] - windowRect->y) * (viewportRect->height / windowRect->height);
		return true;
	}
	return false;
}

AT_API bool ATInputCursor_GetDragDistance(ATMouseInputMapping mouseDragMapping, float* xDrag, float* yDrag)
{
	if (!ATInputCursor_GetCursorDelta(mouseDragMapping, xDrag, yDrag))
	{
		if (g_CursorData->touchCount != 1 || g_CursorData->touchList[0].touchState != ATTIS_Drag)
		{
			return false;
		}
		*xDrag = g_CursorData->touchList[0].currentInputStates[ATITI_Position_X] - g_CursorData->touchList[0].previousInputStates[ATITI_Position_X];
		*yDrag = g_CursorData->touchList[0].currentInputStates[ATITI_Position_Y] - g_CursorData->touchList[0].previousInputStates[ATITI_Position_Y];
		return true;
	}
	return true;
}

AT_API bool ATInputCursor_GetPinchDistance(ATMouseInputMapping mouseDragMapping, float* xDrag, float* yDrag)
{
	if (!ATInputCursor_GetCursorDelta(mouseDragMapping, xDrag, yDrag))
	{
		return false;
	}
	return true;
}
