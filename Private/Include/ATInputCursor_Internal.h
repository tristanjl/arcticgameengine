#ifndef AT_INPUT_CURSOR_INTERNAL_H
#define AT_INPUT_CURSOR_INTERNAL_H

#include "ATInput.h"

#if defined(IPHONE)
#define TRACK_TOUCH_VIA_POSITION
#else
#define TRACK_TOUCH_VIA_IDENTIFIER
#endif

enum ATInputTouchInput
{
	ATITI_Active,
	ATITI_Position_X,
	ATITI_Position_Y,
	ATITI_MAX
};

enum ATInputMouseInput
{
	ATIMI_Button_1,
	ATIMI_Button_2,
	ATIMI_Button_3,
	ATIMI_Position_X,
	ATIMI_Position_Y,
	ATIMI_Mouse_Wheel,
	ATIMI_MAX,
	ATIMI_BUTTON_MAX
};

struct ATInputTouchData;

AT_API void ATInputCursor_UpdatePlatformSpecific();

AT_API void ATInputCursor_UpdateMouseState(ATInputMouseInput mouseInput, int mouseIndex, float inputValue);

// Creates a touch of the given identifier. Destruction occurs via ATInputCursor_Update automatically 
AT_API void ATInputCursor_CreateTouchState(uint32 touchIdentifier, float xPosition, float yPosition);

AT_API ATInputTouchData* ATInputCursor_GetTouchData
(
#if defined(TRACK_TOUCH_VIA_IDENTIFIER)
    uint32 touchIdentifier
#elif defined(TRACK_TOUCH_VIA_POSITION)
    float positionX, float positionY
#endif
);

AT_API void ATInputCursor_UpdateTouchState(ATInputTouchData* data, ATInputTouchInput touchInput, float inputValue);

#endif // AT_INPUT_CURSOR_INTERNAL_H
