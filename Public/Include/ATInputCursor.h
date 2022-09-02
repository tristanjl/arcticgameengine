#ifndef AT_INPUT_CURSOR
#define AT_INPUT_CURSOR

// ATInputCursor funnels all touch and mouse input into a single system

enum ATMouseButton
{
	ATMB_Button1,
	ATMB_Button2,
	ATMB_Button3,
	ATMB_Touch = ATMB_Button1
};

enum ATMouseInputMapping
{
	ATMIM_PrimaryMouseClickDrag,
	ATMIM_SecondaryMouseClickDrag,
	ATMIM_MiddleMouseClickDrag,
	ATMIM_PrimarySecondaryMouseClickDrag,
	ATMIM_MouseWheelMove
};

AT_API bool ATInputCursor_GetMousePosition(float* xPosition, float* yPosition);

AT_API bool ATInputCursor_GetTap(float* xPosition, float* yPosition, ATMouseButton button);

AT_API bool ATInputCursor_GetDragDistance(ATMouseInputMapping mouseDragMapping, float* xDrag, float* yDrag);

AT_API bool ATInputCursor_GetPinchDistance(ATMouseInputMapping mousePinchMapping, float* xPinch, float* yPinch);

#endif // AT_INPUT_CURSOR
