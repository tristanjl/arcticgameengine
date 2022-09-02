#include "AT.h"
#include "ATMath.h"
#include "ATMatrix.h"

AT_API ATMatrix ATMatrix_Create(float m00, float m01, float m02, float m03,
						 float m10, float m11, float m12, float m13,
						 float m20, float m21, float m22, float m23,
						 float m30, float m31, float m32, float m33)
{
	ATMatrix matrix;
	matrix.f[0] = m00;
	matrix.f[1] = m01;
	matrix.f[2] = m02;
	matrix.f[3] = m03;
	matrix.f[4] = m10;
	matrix.f[5] = m11;
	matrix.f[6] = m12;
	matrix.f[7] = m13;
	matrix.f[8] = m20;
	matrix.f[9] = m21;
	matrix.f[10] = m22;
	matrix.f[11] = m23;
	matrix.f[12] = m30;
	matrix.f[13] = m31;
	matrix.f[14] = m32;
	matrix.f[15] = m33;
	return matrix;
}

AT_API ATMatrix ATMatrix_CreateRotationYPR(float yaw, float pitch, float roll)
{
	float cosY = ATMath_Cos(yaw);
	float cosP = ATMath_Cos(pitch);
	float cosR = ATMath_Cos(roll);
	float sinY = ATMath_Sin(yaw);
	float sinP = ATMath_Sin(pitch);
	float sinR = ATMath_Sin(roll);
	ATMatrix matrix;
	matrix.f[0] = cosP * cosY;
	matrix.f[1] = sinP * cosY * sinR - sinY * cosR;
	matrix.f[2] = sinP * cosY * cosR + sinY * sinR;
	matrix.f[3] = 0.0f;
	matrix.f[4] = cosP * sinY;
	matrix.f[5] = sinP * sinY * sinR + cosY * cosR;
	matrix.f[6] = sinP * sinY * cosR + cosY * sinR;;
	matrix.f[7] = 0.0f;
	matrix.f[8] = -sinP;
	matrix.f[9] = cosP * sinR;
	matrix.f[10] = cosP * cosR;
	matrix.f[11] = 0.0f;
	matrix.f[12] = 0.0f;
	matrix.f[13] = 0.0f;
	matrix.f[14] = 0.0f;
	matrix.f[15] = 1.0f;
	return matrix;
}

const ATMatrix ATMatrix::Identity = ATMatrix_Create(1.0f, 0.0f, 0.0f, 0.0f,
													0.0f, 1.0f, 0.0f, 0.0f,
													0.0f, 0.0f, 1.0f, 0.0f,
													0.0f, 0.0f, 0.0f, 1.0f);

void ATMatrix::Multiply(const ATMatrix& matrix)
{
	m[0][0] = m[0][0] * matrix.m[0][0] + m[0][1] * matrix.m[1][0] + m[0][2] * matrix.m[2][0] + m[0][3] * matrix.m[3][0];
	m[0][1] = m[0][0] * matrix.m[0][1] + m[0][1] * matrix.m[1][1] + m[0][2] * matrix.m[2][1] + m[0][3] * matrix.m[3][1];
	m[0][2] = m[0][0] * matrix.m[0][2] + m[0][1] * matrix.m[1][2] + m[0][2] * matrix.m[2][2] + m[0][3] * matrix.m[3][2];
	m[0][3] = m[0][0] * matrix.m[0][3] + m[0][1] * matrix.m[1][3] + m[0][2] * matrix.m[2][3] + m[0][3] * matrix.m[3][3];
	m[1][0] = m[1][0] * matrix.m[0][0] + m[1][1] * matrix.m[1][0] + m[1][2] * matrix.m[2][0] + m[1][3] * matrix.m[3][0];
	m[1][1] = m[1][0] * matrix.m[0][1] + m[1][1] * matrix.m[1][1] + m[1][2] * matrix.m[2][1] + m[1][3] * matrix.m[3][1];
	m[1][2] = m[1][0] * matrix.m[0][2] + m[1][1] * matrix.m[1][2] + m[1][2] * matrix.m[2][2] + m[1][3] * matrix.m[3][2];
	m[1][3] = m[1][0] * matrix.m[0][3] + m[1][1] * matrix.m[1][3] + m[1][2] * matrix.m[2][3] + m[1][3] * matrix.m[3][3];
	m[2][0] = m[2][0] * matrix.m[0][0] + m[2][1] * matrix.m[1][0] + m[2][2] * matrix.m[2][0] + m[2][3] * matrix.m[3][0];
	m[2][1] = m[2][0] * matrix.m[0][1] + m[2][1] * matrix.m[1][1] + m[2][2] * matrix.m[2][1] + m[2][3] * matrix.m[3][1];
	m[2][2] = m[2][0] * matrix.m[0][2] + m[2][1] * matrix.m[1][2] + m[2][2] * matrix.m[2][2] + m[2][3] * matrix.m[3][2];
	m[2][3] = m[2][0] * matrix.m[0][3] + m[2][1] * matrix.m[1][3] + m[2][2] * matrix.m[2][3] + m[2][3] * matrix.m[3][3];
	m[3][0] = m[3][0] * matrix.m[0][0] + m[3][1] * matrix.m[1][0] + m[3][2] * matrix.m[2][0] + m[3][3] * matrix.m[3][0];
	m[3][1] = m[3][0] * matrix.m[0][1] + m[3][1] * matrix.m[1][1] + m[3][2] * matrix.m[2][1] + m[3][3] * matrix.m[3][1];
	m[3][2] = m[3][0] * matrix.m[0][2] + m[3][1] * matrix.m[1][2] + m[3][2] * matrix.m[2][2] + m[3][3] * matrix.m[3][2];
	m[3][3] = m[3][0] * matrix.m[0][3] + m[3][1] * matrix.m[1][3] + m[3][2] * matrix.m[2][3] + m[3][3] * matrix.m[3][3];
}

void ATMatrix::Multiply3x3(const ATMatrix& matrix)
{
	m[0][0] = m[0][0] * matrix.m[0][0] + m[0][1] * matrix.m[1][0] + m[0][2] * matrix.m[2][0];
	m[0][1] = m[0][0] * matrix.m[0][1] + m[0][1] * matrix.m[1][1] + m[0][2] * matrix.m[2][1];
	m[0][2] = m[0][0] * matrix.m[0][2] + m[0][1] * matrix.m[1][2] + m[0][2] * matrix.m[2][2];
	m[1][0] = m[1][0] * matrix.m[0][0] + m[1][1] * matrix.m[1][0] + m[1][2] * matrix.m[2][0];
	m[1][1] = m[1][0] * matrix.m[0][1] + m[1][1] * matrix.m[1][1] + m[1][2] * matrix.m[2][1];
	m[1][2] = m[1][0] * matrix.m[0][2] + m[1][1] * matrix.m[1][2] + m[1][2] * matrix.m[2][2];
	m[2][0] = m[2][0] * matrix.m[0][0] + m[2][1] * matrix.m[1][0] + m[2][2] * matrix.m[2][0];
	m[2][1] = m[2][0] * matrix.m[0][1] + m[2][1] * matrix.m[1][1] + m[2][2] * matrix.m[2][1];
	m[2][2] = m[2][0] * matrix.m[0][2] + m[2][1] * matrix.m[1][2] + m[2][2] * matrix.m[2][2];
}

void ATMatrix::Scale(const ATVector4& scale)
{
	Multiply3x3(ATMatrix_Create(scale.v.x, 0.0f, 0.0f, 0.0f,
		0.0f, scale.v.y, 0.0f, 0.0f,
		0.0f, 0.0f, scale.v.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f));
}

void ATMatrix::Inverse()
{
}
