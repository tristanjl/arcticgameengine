#include "AT.h"
#include "ATMath.h"
#include "ATTime.h"

#include "math.h"
#include "cstdlib"
#include "stdlib.h"

const float ATMath_PI = 3.14159265358979323846f;

float ATMath_Round(float value)
{
	return floorf(value + 0.5f);
}

float ATMath_Truncate(float value)
{
	return floorf(value);
}

int32 ATMath_Random()
{
	static bool seeded = false;
	if (!seeded)
	{
		seeded = true;
		srand((uint32)ATTime_GetSystemTimeInMicroseconds());
	}
	return rand();
}

float ATMath_SqaureRoot(float value)
{
	return sqrtf(value);
}

float ATMath_Sin(float radians)
{
	return sinf(radians);
}

float ATMath_Cos(float radians)
{
	return cosf(radians);
}

float ATMath_Tan(float radians)
{
	return tanf(radians);
}

float ATMath_ASin(float ratio)
{
	return asinf(ratio);
}

float ATMath_ACos(float ratio)
{
	return acosf(ratio);
}

float ATMath_ATan(float ratio)
{
	return atanf(ratio);
}

float ATMath_DegToRad(float degrees)
{
	return degrees * ATMath_PI / 180.0f;
}

float ATMath_RadToDeg(float radians)
{
	return radians * 180.0f / ATMath_PI;
}

size_t ATMath_GetNextPowerOf2(uint32 value)
{
	--value;
	value = (value >> 1) | value;
	value = (value >> 2) | value;
	value = (value >> 4) | value;
	value = (value >> 8) | value;
	value = (value >> 16) | value;
	++value;
	return value;
}

uint32 ATMath_BitSetCount32(uint32 value)
{
	value = value - ((value >> 1) & 0x55555555);
	value = (value & 0x33333333) + ((value >> 2) & 0x33333333);
	value = value + (value >> 8);
	value = value + (value >> 16);
	return value & 0x0000003F;
}
