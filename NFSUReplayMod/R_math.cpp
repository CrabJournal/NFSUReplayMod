#include "stdafx.h"
#include "R_math.h"
#include "FromGame.h"

extern "C" int __fltused;

// "Compress" matrix
void RotationMatrixToRotation(const float Matrix[4][4], vector3* rotation) {
	// remake to integer, compiler using much useless movss instructions 
	rotation->x = SetCosSignTo(Matrix[1][0], Matrix[0][0]);		//  sin x * cos y and sign bit from cos x * cos y
	rotation->y = Matrix[2][0];									// -sin y
	rotation->z = SetCosSignTo(Matrix[2][1], Matrix[2][2]);		//  sin z * cos y and sign bit from cos z * cos y
}
void RotationToRotationMatrix(float Matrix[4][4], const vector3* rotation) {
	float ucosY = GetSinCos(rotation->y);
	vector3a rbXZ  = (*(vector3*)rotation >> 1) << 1; // zeroing last bit of mantissa for x and z, do not use y
	vector3  sins = { FitIntoOne(rbXZ.x / ucosY), -rotation->y, FitIntoOne(rbXZ.z / ucosY) };
	vector3a coss = SetCosSignFromSrcSSE(*rotation, GetSinCosSSE(sins));

	float cosXsinY = coss.x * sins.y;
	float sinXsinZ = sins.x * sins.z;
	float sinXcosZ = sins.x * coss.z;

	Matrix[0][0] = coss.x * ucosY;
	Matrix[0][1] = cosXsinY * sins.z - sinXcosZ;
	Matrix[0][2] = cosXsinY * coss.z + sinXsinZ;
	Matrix[1][0] = rbXZ.x;
	Matrix[1][1] = sinXsinZ * sins.y + coss.x * coss.z;
	Matrix[1][2] = sinXcosZ * sins.y - coss.x * sins.z;
	Matrix[2][0] = rotation->y;
	Matrix[2][1] = rbXZ.z;
	Matrix[2][2] = ucosY * coss.z;
}