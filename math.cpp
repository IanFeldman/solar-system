#include "Math.h"

const Vector2 Vector2::Zero(0.0f, 0.0f);
const Vector2 Vector2::UnitX(1.0f, 0.0f);
const Vector2 Vector2::UnitY(0.0f, 1.0f);
const Vector2 Vector2::NegUnitX(-1.0f, 0.0f);
const Vector2 Vector2::NegUnitY(0.0f, -1.0f);

const Vector3 Vector3::Zero(0.0f, 0.0f, 0.f);
const Vector3 Vector3::UnitX(1.0f, 0.0f, 0.0f);
const Vector3 Vector3::UnitY(0.0f, 1.0f, 0.0f);
const Vector3 Vector3::UnitZ(0.0f, 0.0f, 1.0f);
const Vector3 Vector3::NegUnitX(-1.0f, 0.0f, 0.0f);
const Vector3 Vector3::NegUnitY(0.0f, -1.0f, 0.0f);
const Vector3 Vector3::NegUnitZ(0.0f, 0.0f, -1.0f);
const Vector3 Vector3::Infinity(Math::Infinity, Math::Infinity, Math::Infinity);
const Vector3 Vector3::NegInfinity(Math::NegInfinity, Math::NegInfinity, Math::NegInfinity);

static float m3Ident[3][3] =
{
	{ 1.0f, 0.0f, 0.0f },
	{ 0.0f, 1.0f, 0.0f },
	{ 0.0f, 0.0f, 1.0f }
};
const Matrix3 Matrix3::Identity(m3Ident);

void Matrix3::Invert()
{
	// computes the inverse of a matrix m
	double det = mat[0][0] * (mat[1][1] * mat[2][2] - mat[2][1] * mat[1][2]) -
		mat[0][1] * (mat[1][0] * mat[2][2] - mat[1][2] * mat[2][0]) +
		mat[0][2] * (mat[1][0] * mat[2][1] - mat[1][1] * mat[2][0]);

	double invdet = 1 / det;

	Matrix3 minv; // inverse of matrix m
	minv.mat[0][0] = (mat[1][1] * mat[2][2] - mat[2][1] * mat[1][2]) * invdet;
	minv.mat[0][1] = (mat[0][2] * mat[2][1] - mat[0][1] * mat[2][2]) * invdet;
	minv.mat[0][2] = (mat[0][1] * mat[1][2] - mat[0][2] * mat[1][1]) * invdet;
	minv.mat[1][0] = (mat[1][2] * mat[2][0] - mat[1][0] * mat[2][2]) * invdet;
	minv.mat[1][1] = (mat[0][0] * mat[2][2] - mat[0][2] * mat[2][0]) * invdet;
	minv.mat[1][2] = (mat[1][0] * mat[0][2] - mat[0][0] * mat[1][2]) * invdet;
	minv.mat[2][0] = (mat[1][0] * mat[2][1] - mat[2][0] * mat[1][1]) * invdet;
	minv.mat[2][1] = (mat[2][0] * mat[0][1] - mat[0][0] * mat[2][1]) * invdet;
	minv.mat[2][2] = (mat[0][0] * mat[1][1] - mat[1][0] * mat[0][1]) * invdet;
	
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			mat[i][j] = minv.mat[i][j];
		}
	}
}

Vector2 Vector2::Transform(const Vector2& vec, const Matrix3& mat, float w /*= 1.0f*/)
{
	Vector2 retVal;
	retVal.x = vec.x * mat.mat[0][0] + vec.y * mat.mat[1][0] + w * mat.mat[2][0];
	retVal.y = vec.x * mat.mat[0][1] + vec.y * mat.mat[1][1] + w * mat.mat[2][1];
	//ignore w since we aren't returning a new value for it...
	return retVal;
}
