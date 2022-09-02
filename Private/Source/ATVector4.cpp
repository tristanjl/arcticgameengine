#include "AT.h"
#include "ATMath.h"
#include "ATMatrix.h"

ATVector4 ATVector4_Create(float x, float y, float z, float w)
{
	ATVector4 vector4;
	vector4.v.x = x;
	vector4.v.y = y;
	vector4.v.z = z;
	vector4.v.w = w;
	return vector4;
}

ATVector4 ATVector4_TransformByMatrix(const ATVector4 &vector, const ATMatrix &transformation)
{
	return ATVector4_Create
		(
		vector.v.x * transformation.m[0][0] + vector.v.y * transformation.m[1][0] + vector.v.z * transformation.m[2][0] + vector.v.w * transformation.m[3][0],
		vector.v.x * transformation.m[0][1] + vector.v.y * transformation.m[1][1] + vector.v.z * transformation.m[2][1] + vector.v.w * transformation.m[3][1],
		vector.v.x * transformation.m[0][2] + vector.v.y * transformation.m[1][2] + vector.v.z * transformation.m[2][2] + vector.v.w * transformation.m[3][2]
	);
}

float ATVector4::Dot(const ATVector4& dotVector) const
{
	return (v.x * dotVector.v.x) + (v.y * dotVector.v.y) + (v.z * dotVector.v.z);
}

ATVector4 ATVector4::Cross(const ATVector4& crossVector) const
{
	return ATVector4_Create(v.y * crossVector.v.z - crossVector.v.y * v.z,
		v.z * crossVector.v.x - crossVector.v.z * v.x,
		v.x * crossVector.v.y - crossVector.v.x * v.y);
}

float ATVector4::Normalise()
{
	float length = GetLength();

	v.x = v.x / length;
	v.y = v.y / length;
	v.z = v.z / length;

	return length;
}

float ATVector4::GetLength() const
{
	return ATMath_SqaureRoot((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

float ATVector4::GetLengthSquared() const
{
	return (v.x * v.x) + (v.y * v.y) + (v.z * v.z);
}

float ATVector4::GetDistanceSquared(const ATVector4& toVector) const
{
	return ((*this) - toVector).GetLengthSquared();
}

ATVector4 ATVector4::operator +(const ATVector4& additionVector) const
{
	return ATVector4_Create(v.x + additionVector.v.x, v.y + additionVector.v.y, v.z + additionVector.v.z);
}

ATVector4 ATVector4::operator -(const ATVector4& minusVector) const
{
	return ATVector4_Create(v.x - minusVector.v.x, v.y - minusVector.v.y, v.z - minusVector.v.z);
}

ATVector4 ATVector4::operator *(float multiplier) const
{
	return ATVector4_Create(v.x * multiplier, v.y * multiplier, v.z * multiplier);
}

ATVector4 ATVector4::operator /(float divisor) const
{
	return ATVector4_Create(v.x / divisor, v.y / divisor, v.z / divisor);
}

void ATVector4::operator +=(const ATVector4& additionVector)
{
	v.x = v.x + additionVector.v.x;
	v.y = v.y + additionVector.v.y;
	v.z = v.z + additionVector.v.z;
}

void ATVector4::operator -=(const ATVector4& minusVector)
{
	v.x = v.x - minusVector.v.x;
	v.y = v.y - minusVector.v.y;
	v.z = v.z - minusVector.v.z;
}

void ATVector4::operator *=(float multiplier)
{
	v.x = v.x * multiplier;
	v.y = v.y * multiplier;
	v.z = v.z * multiplier;
}

void ATVector4::operator /=(float divisor)
{
	v.x = v.x / divisor;
	v.y = v.y / divisor;
	v.z = v.z / divisor;
}

bool ATVector4::operator ==(const ATVector4& vector) const
{
	return v.x == vector.v.x && v.y == vector.v.y && v.z == vector.v.z && v.w == vector.v.w;
}

bool ATVector4::operator !=(const ATVector4& vector) const
{
	return v.x != vector.v.x || v.y != vector.v.y || v.z != vector.v.z || v.w != vector.v.w;
}

const ATVector4 ATVector4::White = ATVector4_Create(1.0f, 1.0f, 1.0f, 1.0f);
const ATVector4 ATVector4::Black = ATVector4_Create(0.0f, 0.0f, 0.0f, 1.0f);
const ATVector4 ATVector4::Red   = ATVector4_Create(1.0f, 0.0f, 0.0f, 1.0f);
const ATVector4 ATVector4::Green = ATVector4_Create(0.0f, 1.0f, 0.0f, 1.0f);
const ATVector4 ATVector4::Blue  = ATVector4_Create(0.0f, 0.0f, 1.0f, 1.0f);
const ATVector4 ATVector4::Zero  = ATVector4_Create(0.0f, 0.0f, 0.0f, 1.0f);
const ATVector4 ATVector4::One   = ATVector4_Create(1.0f, 1.0f, 1.0f, 1.0f);
const ATVector4 ATVector4::Up    = ATVector4_Create(0.0f, 1.0f, 0.0f, 1.0f);
const ATVector4 ATVector4::Down  = ATVector4_Create(0.0f, -1.0f, 1.0f, 1.0f);
