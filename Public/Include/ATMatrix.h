#ifndef AT_MATRIX_H
#define AT_MATRIX_H

#include "ATVector4.h"

struct ATMatrix
{
	union
	{
		float f[16];
		float m[4][4];
		ATVector4 vector[4];
	};

	void Translate(const ATVector4& translation)
	{
		vector[3] += translation;
	}

	void Translate(float x, float y, float z)
	{
		f[12] += x;
		f[13] += y;
		f[14] += z;
	}

	void SetTranslation(const ATVector4& translation)
	{
		vector[3] = translation;
	}

	void SetTranslation(float x, float y, float z)
	{
		f[12] = x;
		f[13] = y;
		f[14] = z;
	}

	const ATVector4& GetTranslation()
	{
		return vector[3];
	}

	void Multiply(const ATMatrix& matrix);

	void Multiply3x3(const ATMatrix& matrix);

	void Scale(const ATVector4& scale);

	void Scale(float scale);

	void Inverse();

	static const ATMatrix Identity;
};

AT_API ATMatrix ATMatrix_Create(float m00, float m01, float m02, float m03,
						 float m10, float m11, float m12, float m13,
						 float m20, float m21, float m22, float m23,
						 float m30, float m31, float m32, float m33);

AT_API ATMatrix ATMatrix_CreateRotationYPR(float yaw, float pitch, float roll);

#endif //AT_MATRIX_H
