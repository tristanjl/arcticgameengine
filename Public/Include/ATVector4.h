#ifndef VECTOR_H
#define VECTOR_H

#ifdef WINDOWS_STORE
#include <DirectXMath.h>
#endif

struct ATMatrix;

ATALIGN_BEGIN(16)
struct ATVector4
{
	union
	{
		float f[4];
#ifdef WINDOWS_STORE
		struct { float x, y, z, w; } v;
		DirectX::XMVECTOR vi;
#else
		struct { float x, y, z, w; } v;
		struct { char __raw[16]; } vi;
#endif
	};

	float Dot(const ATVector4& dotVector) const;
	ATVector4 Cross(const ATVector4& crossVector) const;

	float Normalise();
	float GetLength() const;
	float GetLengthSquared() const;
	float GetDistanceSquared(const ATVector4& toVector) const;

	ATVector4 operator +(const ATVector4& additionVector) const;
	ATVector4 operator -(const ATVector4& minusVector) const;
	ATVector4 operator *(float multiplier) const;
	ATVector4 operator /(float divisor) const;

	void operator +=(const ATVector4& addedVector);
	void operator -=(const ATVector4& minusVector);
	void operator *=(float multiplier);
	void operator /=(float divisor);

	bool operator ==(const ATVector4& vector) const;
	bool operator !=(const ATVector4& vector) const;

	static const ATVector4 White;
	static const ATVector4 Black;
	static const ATVector4 Red;
	static const ATVector4 Green;
	static const ATVector4 Blue;
	static const ATVector4 Zero;
	static const ATVector4 One;
	static const ATVector4 Up;
	static const ATVector4 Down;
} ATALIGN_END(16);

ATVector4 ATVector4_Create(float x, float y, float z, float w = 0.0f);

ATVector4 ATVector4_TransformByMatrix(const ATVector4 &vector, const ATMatrix &transformation);

#endif //VECTOR_H
