#ifndef AT_MATH_H
#define AT_MATH_H

#include "ATTypes.h"
extern const float ATMath_PI;

template <typename T>
T ATMath_Min(T value1, T value2)
{
    return value1 < value2 ? value1 : value2;
}

template <typename T>
T ATMath_Max(T value1, T value2)
{
    return value1 > value2 ? value1 : value2;
}

template <typename T>
T ATMath_Clamp(T min, T value, T max)
{
    return ATMath_Min(ATMath_Max(min, value), max);
}

float ATMath_Round(float value);

float ATMath_Truncate(float value);

int32 ATMath_Random();

float ATMath_SqaureRoot(float value);

float ATMath_Sin(float radians);

float ATMath_Cos(float radians);

float ATMath_Tan(float radians);

float ATMath_ASin(float radians);

float ATMath_ACos(float radians);

float ATMath_ATan(float radians);

float ATMath_DegToRad(float degrees);

size_t ATMath_GetNextPowerOf2(uint32 value);

uint32 ATMath_BitSetCount32(uint32 value);

#endif // AT_MATH_H
