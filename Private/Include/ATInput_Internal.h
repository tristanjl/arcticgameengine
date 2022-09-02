#ifndef AT_INPUT_INTERNAL_H
#define AT_INPUT_INTERNAL_H

#include "ATInput.h"

struct GamepadData
{
	float inputStates[ATGIM_MAX];
	float previousInputStates[ATGIM_MAX];
	bool active;
};

struct KeyboardData
{
	float inputStates[ATKIM_MAX];
	float previousInputStates[ATKIM_MAX];
	bool active;
};

extern GamepadData* s_GamepadData;
extern KeyboardData* s_KeyboardData;

#endif //AT_INPUT_INTERNAL_H
